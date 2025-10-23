#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BoardManager.h"

ATile::ATile()
{
    PrimaryActorTick.bCanEverTick = false;

    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    RootComponent = TileMesh;

    bIsOccupied = false;
    OccupiedUnit = nullptr;
    OriginalColor = FLinearColor::White;
    BoardManagerRef = nullptr;
    DynMat = nullptr;

    TileMesh->SetMobility(EComponentMobility::Static);
    TileMesh->SetGenerateOverlapEvents(true);
    TileMesh->bSelectable = true;

    // If you want to use OnClicked binding in C++:
    // TileMesh->OnClicked.AddDynamic(this, &ATile::NotifyBoardManagerClicked);
}

void ATile::BeginPlay()
{
    Super::BeginPlay();

    OriginalColor = FLinearColor(1.f, 1.f, 1.f, 1.f);

    if (TileMesh)
    {
        DynMat = TileMesh->CreateAndSetMaterialInstanceDynamic(0);
        if (DynMat)
        {
            DynMat->SetVectorParameterValue(FName("BaseColor"), OriginalColor);
        }
    }
}

void ATile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ATile::SetTileColor(const FLinearColor& NewColor)
{
    if (DynMat)
    {
        DynMat->SetVectorParameterValue(FName("BaseColor"), NewColor);
    }
}

void ATile::ResetTileColor()
{
    SetTileColor(OriginalColor);
}

void ATile::NotifyBoardManagerClicked()
{
    if (BoardManagerRef)
    {
        BoardManagerRef->HandleTileClicked(this);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[ATile] BoardManagerRef is null on tile %s"), *GetName());
    }
}

void ATile::SetTileHighlight(bool bHighlight)
{
    if (!DynMat)
    {
        DynMat = TileMesh->CreateAndSetMaterialInstanceDynamic(0);
    }

    if (!DynMat) return;

    if (bHighlight)
        DynMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor::Green); // íuÇØÇÈ
    else
        DynMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.2f, 0.4f, 1.f, 1.f));// å≥ÇÃêF
}
