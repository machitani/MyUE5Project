#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ATile::ATile()
{
    PrimaryActorTick.bCanEverTick = false;

    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    RootComponent = TileMesh;

    bIsOccupied = false;
    OccupiedUnit = nullptr;
    OriginalColor = FLinearColor::White;
}

void ATile::BeginPlay()
{
    Super::BeginPlay();
    OriginalColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
}

void ATile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ATile::SetTileColor(const FLinearColor& NewColor)
{
    if (!TileMesh) return;

    UMaterialInstanceDynamic* DynMat = TileMesh->CreateAndSetMaterialInstanceDynamic(0);
    if (DynMat)
    {
        DynMat->SetVectorParameterValue(FName("BaseColor"), NewColor);
    }
}

void ATile::ResetTileColor()
{
    SetTileColor(OriginalColor);
}
