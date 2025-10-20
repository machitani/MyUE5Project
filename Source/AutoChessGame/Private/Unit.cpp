#include "Unit.h"
#include "Components/StaticMeshComponent.h"

AUnit::AUnit()
{
    PrimaryActorTick.bCanEverTick = false;

    UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
    RootComponent = UnitMesh;

    CurrentTile = nullptr;
}
