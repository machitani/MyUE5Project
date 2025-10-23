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

    // �h���b�O�J�n���Ƀ��j�b�g�̃R���W�����𖳌���
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // �}�E�X�Ƃ̃I�t�Z�b�g���v�Z
    DragOffset = GetActorLocation() - MouseWorld;
}

void AUnit::EndDrag()
{
    bIsDragging = false;

    // �h���b�O�I�����Ƀ��j�b�g�̃R���W���������ɖ߂�
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AUnit::UpdateDrag(const FVector& MouseWorld)
{
    FVector Target = MouseWorld + DragOffset;
    // z�͈ێ�
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

    // �U���N�[���_�E�����X�V
    TimeSinceLastAttack += DeltaTime;

    // �N�[���_�E�����Ȃ�U�����Ȃ�
    if (TimeSinceLastAttack < AttackInterval)
        return;

    // �ł��߂��G��T��
    AUnit* ClosestEnemy = nullptr;
    float ClosestDist = FLT_MAX;

    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Other = *It;
        if (Other == this) continue;          // �������g�͏��O
        if (Other->Team == Team) continue;    // ���w�c�͏��O
        if (Other->HP <= 0.f) continue;      // ���S���j�b�g�͏��O

        float Distance = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
        if (Distance <= Range && Distance < ClosestDist)
        {
            ClosestDist = Distance;
            ClosestEnemy = Other;
        }
    }

    // �ł��߂��G��������΍U��
    if (ClosestEnemy)
    {
        AttackTarget(ClosestEnemy);
        TimeSinceLastAttack = 0.0f; // �U����N�[���_�E�����Z�b�g
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

    // �^�C����L����
    if (CurrentTile)
    {
        CurrentTile->bIsOccupied = false;
        CurrentTile->OccupiedUnit = nullptr;
        CurrentTile = nullptr;
    }

    // OwningBoardManager ������� PlayerUnits ����폜�i�v���C���[�w�c�̂݁j
    if (OwningBoardManager && Team == EUnitTeam::Player)
    {
        OwningBoardManager->PlayerUnits.Remove(this);
    }

    Destroy();
}
