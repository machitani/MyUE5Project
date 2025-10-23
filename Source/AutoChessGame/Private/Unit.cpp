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
