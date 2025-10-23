#include "Unit.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

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
