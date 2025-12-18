#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BoardManager.h"

ATile::ATile()
{
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = RootScene;

    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    TileMesh->SetupAttachment(RootComponent);

    HighlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HighlightMesh"));
    HighlightMesh->SetupAttachment(RootComponent);

    HighlightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HighlightMesh->SetGenerateOverlapEvents(false);

    HighlightMesh->SetRelativeLocation(FVector(0.f, 0.f, 1.f));
    HighlightMesh->SetVisibility(false);

    // ★クリック拾えるように（任意だけどおすすめ）
    if (TileMesh)
    {
        TileMesh->SetGenerateOverlapEvents(false);
        TileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        TileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
        TileMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
        TileMesh->OnClicked.AddDynamic(this, &ATile::OnTileMeshClicked);
    }
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

void ATile::InitTile(int32 InRow, int32 InCol, bool bInIsPlayerTile, ABoardManager* InBoardManager)
{
    Row = InRow;
    Col = InCol;
    bIsPlayerTile = bInIsPlayerTile;
    BoardManagerRef = InBoardManager;
}

void ATile::SetOccupiedUnit(AUnit* Unit)
{
    OccupiedUnit = Unit;
    bIsOccupied = (Unit != nullptr);
}

void ATile::ClearOccupiedUnit()
{
    OccupiedUnit = nullptr;
    bIsOccupied = false;
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
    if (HighlightMesh)
    {
        HighlightMesh->SetVisibility(bHighlight);
    }
}

FVector ATile::GetTileCenterWorld() const
{
    if (TileMesh)
    {
        // メッシュの実際の中心（ワールド座標）
        return TileMesh->Bounds.Origin;
    }
    return GetActorLocation();
}

float ATile::GetTileSnapRadius() const
{
    if (TileMesh)
    {
        const FBoxSphereBounds& B = TileMesh->Bounds;
        // XY の半径（大きい方）をスナップ許容半径にする
        return FMath::Max(B.BoxExtent.X, B.BoxExtent.Y);
    }
    return 100.f; // 保険
}

void ATile::OnTileMeshClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
    NotifyBoardManagerClicked();
}