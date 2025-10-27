#include "Unit.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"

AUnit::AUnit()
{
    PrimaryActorTick.bCanEverTick = true;

    UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
    RootComponent = UnitMesh;

    bIsDragging = false;
    DragOffset = FVector::ZeroVector;

    UnitMesh->SetMobility(EComponentMobility::Movable);
    UnitMesh->SetGenerateOverlapEvents(true);
    UnitMesh->bSelectable = true;

    CurrentTile = nullptr;
    OriginalLocation = FVector::ZeroVector;
    bCanDrag = true;
    TimeSinceLastAttack = 0.0f;
}

void AUnit::StartDrag(const FVector& MouseWorld)
{
    if (!bCanDrag) return;

    bIsDragging = true;
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    DragOffset = GetActorLocation() - MouseWorld;
}

void AUnit::EndDrag()
{
    bIsDragging = false;
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AUnit::UpdateDrag(const FVector& MouseWorld)
{
    if (!bIsDragging) return;

    FVector Target = MouseWorld + DragOffset;
    Target.Z = GetActorLocation().Z; // 高さ維持
    SetActorLocation(Target);
}

void AUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ドラッグ中はマウス追従
    if (bIsDragging)
    {
        // Tick 内でマウス位置取得
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            FVector MouseWorld, MouseDir;
            if (PC->DeprojectMousePositionToWorld(MouseWorld, MouseDir))
            {
                UpdateDrag(MouseWorld);
            }
        }
        return;
    }

    // 戦闘フェーズ中は敵を探索して行動
    if (OwningBoardManager && OwningBoardManager->CurrentPhase == EGamePhase::Battle)
    {
        CheckForTarget(DeltaTime);
    }
}

void AUnit::CheckForTarget(const float DeltaTime)
{
    if (!OwningBoardManager) return;
    if (OwningBoardManager->CurrentPhase != EGamePhase::Battle) return;

    // 攻撃クールダウン更新
    TimeSinceLastAttack += DeltaTime;

    if (TimeSinceLastAttack < AttackInterval) return;

    // 最も近い敵を探索
    AUnit* ClosestEnemy = nullptr;
    float ClosestDist = FLT_MAX;

    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Other = *It;
        if (Other == this) continue;
        if (Other->Team == Team) continue;
        if (Other->HP <= 0.f) continue;

        float Distance = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
        if (Distance < ClosestDist)
        {
            ClosestDist = Distance;
            ClosestEnemy = Other;
        }
    }

    if (!ClosestEnemy) return;

    if (ClosestDist <= Range)
    {
        // 射程内なら攻撃
        AttackTarget(ClosestEnemy);
        TimeSinceLastAttack = 0.f;
    }
    else
    {
        // 射程外なら移動
        FVector NewLocation = FMath::VInterpConstantTo(
            GetActorLocation(),
            ClosestEnemy->GetActorLocation(),
            DeltaTime,
            MoveSpeed // 1秒あたりの移動距離
        );
        SetActorLocation(NewLocation);
    }
}

void AUnit::AttackTarget(AUnit* Target)
{
    if (!Target) return;

    Target->HP -= Attack;

    UE_LOG(LogTemp, Warning, TEXT("%s attacked %s (HP: %.1f)"),
        *GetName(), *Target->GetName(), Target->HP);

    if (Target->HP <= 0.f)
    {
        Target->OnDeath();
    }
}

void AUnit::OnDeath()
{
    UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());

    if (CurrentTile)
    {
        CurrentTile->bIsOccupied = false;
        CurrentTile->OccupiedUnit = nullptr;
        CurrentTile = nullptr;
    }

    // プレイヤー陣営の場合は BoardManager の配列から削除
    if (OwningBoardManager && Team == EUnitTeam::Player)
    {
        OwningBoardManager->PlayerUnits.Remove(this);
    }

    Destroy();
}
