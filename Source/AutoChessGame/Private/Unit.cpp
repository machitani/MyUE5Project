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
}

void AUnit::StartDrag(const FVector& MouseWorld)
{
    bIsDragging = true;

    // ドラッグ開始時にユニットのコリジョンを無効化
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // マウスとのオフセットを計算
    DragOffset = GetActorLocation() - MouseWorld;
}

void AUnit::EndDrag()
{
    bIsDragging = false;

    // ドラッグ終了時にユニットのコリジョンを元に戻す
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AUnit::UpdateDrag(const FVector& MouseWorld)
{
    FVector Target = MouseWorld + DragOffset;
    // zは維持
    Target.Z = GetActorLocation().Z;
    SetActorLocation(Target);
}

void AUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsDragging)
    {
        CheckForTarget(DeltaTime);
    }
}

void AUnit::CheckForTarget(float DeltaTime)
{
    if (!OwningBoardManager) return;

    // 攻撃クールダウンを更新
    TimeSinceLastAttack += DeltaTime;

    // クールダウン中なら攻撃しない
    if (TimeSinceLastAttack < AttackInterval)
        return;

    // 最も近い敵を探索
    AUnit* ClosestEnemy = nullptr;
    float ClosestDist = FLT_MAX;

    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Other = *It;
        if (Other == this) continue;          // 自分自身は除外
        if (Other->Team == Team) continue;    // 同陣営は除外
        if (Other->HP <= 0.f) continue;      // 死亡ユニットは除外

        float Distance = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
        if (Distance <= Range && Distance < ClosestDist)
        {
            ClosestDist = Distance;
            ClosestEnemy = Other;
        }
    }

    // 最も近い敵が見つかれば攻撃
    if (ClosestEnemy)
    {
        AttackTarget(ClosestEnemy);
        TimeSinceLastAttack = 0.0f; // 攻撃後クールダウンリセット
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

    // タイル占有解除
    if (CurrentTile)
    {
        CurrentTile->bIsOccupied = false;
        CurrentTile->OccupiedUnit = nullptr;
        CurrentTile = nullptr;
    }

    // OwningBoardManager があれば PlayerUnits から削除（プレイヤー陣営のみ）
    if (OwningBoardManager && Team == EUnitTeam::Player)
    {
        OwningBoardManager->PlayerUnits.Remove(this);
    }

    Destroy();
}
