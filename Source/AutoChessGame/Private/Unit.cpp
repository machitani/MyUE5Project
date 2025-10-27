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
    Target.Z = GetActorLocation().Z; // �����ێ�
    SetActorLocation(Target);
}

void AUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // �h���b�O���̓}�E�X�Ǐ]
    if (bIsDragging)
    {
        // Tick ���Ń}�E�X�ʒu�擾
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

    // �퓬�t�F�[�Y���͓G��T�����čs��
    if (OwningBoardManager && OwningBoardManager->CurrentPhase == EGamePhase::Battle)
    {
        CheckForTarget(DeltaTime);
    }
}

void AUnit::CheckForTarget(const float DeltaTime)
{
    if (!OwningBoardManager) return;
    if (OwningBoardManager->CurrentPhase != EGamePhase::Battle) return;

    // �U���N�[���_�E���X�V
    TimeSinceLastAttack += DeltaTime;

    if (TimeSinceLastAttack < AttackInterval) return;

    // �ł��߂��G��T��
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
        // �˒����Ȃ�U��
        AttackTarget(ClosestEnemy);
        TimeSinceLastAttack = 0.f;
    }
    else
    {
        // �˒��O�Ȃ�ړ�
        FVector NewLocation = FMath::VInterpConstantTo(
            GetActorLocation(),
            ClosestEnemy->GetActorLocation(),
            DeltaTime,
            MoveSpeed // 1�b������̈ړ�����
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

    // �v���C���[�w�c�̏ꍇ�� BoardManager �̔z�񂩂�폜
    if (OwningBoardManager && Team == EUnitTeam::Player)
    {
        OwningBoardManager->PlayerUnits.Remove(this);
    }

    Destroy();
}
