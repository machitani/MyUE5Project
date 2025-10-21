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
}

void AUnit::BeginPlay()
{
    Super::BeginPlay();
}

void AUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 実際の追従は PlayerController の Tick から SetActorLocation しても良いが、
    // ここでも受け取ったマウスワールド位置で更新するパターンにしておく（UpdateDrag を呼んでください）
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
    // zは維持
    Target.Z = GetActorLocation().Z;
    SetActorLocation(Target);
}
