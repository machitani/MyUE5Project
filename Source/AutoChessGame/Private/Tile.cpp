#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BoardManager.h"

ATile::ATile()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    TileMesh->SetupAttachment(RootComponent);

    HighlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HighlightMesh"));
    HighlightMesh->SetupAttachment(RootComponent);

    HighlightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HighlightMesh->SetGenerateOverlapEvents(false);

    // ちょっとだけ浮かせて Z ファイト防止
    HighlightMesh->SetRelativeLocation(FVector(0.f, 0.f, 1.f));

    // 最初は非表示
    HighlightMesh->SetVisibility(false);
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
