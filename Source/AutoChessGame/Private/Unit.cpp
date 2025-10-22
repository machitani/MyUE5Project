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

void AUnit::BeginPlay()
{
    Super::BeginPlay();
}

void AUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ���ۂ̒Ǐ]�� PlayerController �� Tick ���� SetActorLocation ���Ă��ǂ����A
    // �����ł��󂯎�����}�E�X���[���h�ʒu�ōX�V����p�^�[���ɂ��Ă����iUpdateDrag ���Ă�ł��������j
}

void AUnit::StartDrag(const FVector& MouseWorld)
{
    bIsDragging = true;
    DragOffset = GetActorLocation() - MouseWorld;
}

void AUnit::EndDrag()
{
    bIsDragging = false;
}

void AUnit::UpdateDrag(const FVector& MouseWorld)
{
    FVector Target = MouseWorld + DragOffset;
    // z�͈ێ�
    Target.Z = GetActorLocation().Z;
    SetActorLocation(Target);
}
