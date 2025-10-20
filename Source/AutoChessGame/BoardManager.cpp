#include "BoardManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ABoardManager::ABoardManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABoardManager::BeginPlay()
{
    Super::BeginPlay();

    GenerateBoard();
    SpawnInitialUnits();
}

void ABoardManager::GenerateBoard()
{
    if (!TileClass) return;

    FVector Origin(0, 0, 0);
    const float BoardGap = 0.f;

    // プレイヤーボード
    for (int32 Row = 0; Row < Rows; Row++)
    {
        for (int32 Col = 0; Col < Columns; Col++)
        {
            FVector SpawnLocation = Origin + FVector(Col * TileSpacing, Row * TileSpacing, 0.f);
            FActorSpawnParameters Params;
            ATile* Tile = GetWorld()->SpawnActor<ATile>(TileClass, SpawnLocation, FRotator::ZeroRotator, Params);
            if (Tile)
            {
                Tile->SetTileColor(FLinearColor(0.2f, 0.4f, 1.0f, 1.0f));
                PlayerTiles.Add(Tile);
            }
        }
    }

    // 敵ボード
    for (int32 Row = 0; Row < Rows; Row++)
    {
        for (int32 Col = 0; Col < Columns; Col++)
        {
            FVector SpawnLocation = Origin + FVector(Col * TileSpacing, (Row + Rows) * TileSpacing + BoardGap, 0.f);
            FActorSpawnParameters Params;
            ATile* Tile = GetWorld()->SpawnActor<ATile>(TileClass, SpawnLocation, FRotator::ZeroRotator, Params);
            if (Tile)
            {
                Tile->SetTileColor(FLinearColor(1.0f, 0.3f, 0.3f, 1.0f));
                EnemyTiles.Add(Tile);
            }
        }
    }
}

void ABoardManager::SpawnInitialUnits()
{
    if (!PlayerUnitClass || !EnemyUnitClass) return;

    int32 CenterX = Columns / 2;
    int32 CenterY = Rows / 2;

    int32 PlayerIndex = CenterY * Columns + CenterX;
    int32 EnemyIndex = CenterY * Columns + CenterX;

    if (PlayerTiles.IsValidIndex(PlayerIndex))
    {
        FVector SpawnLocation = PlayerTiles[PlayerIndex]->GetActorLocation() + FVector(0, 0, 150);
        FRotator SpawnRotation(0, 90, 0);
        FActorSpawnParameters Params;
        Params.Owner = this;

        AUnit* PlayerUnit = GetWorld()->SpawnActor<AUnit>(PlayerUnitClass, SpawnLocation, SpawnRotation, Params);
        if (PlayerUnit)
        {
            PlayerUnit->CurrentTile = PlayerTiles[PlayerIndex];
            PlayerTiles[PlayerIndex]->OccupiedUnit = PlayerUnit;
            PlayerTiles[PlayerIndex]->bIsOccupied = true;
        }
    }

    if (EnemyTiles.IsValidIndex(EnemyIndex))
    {
        FVector SpawnLocation = EnemyTiles[EnemyIndex]->GetActorLocation() + FVector(0, 0, 150);
        FRotator SpawnRotation(0, 270, 0);
        FActorSpawnParameters Params;
        Params.Owner = this;

        AUnit* EnemyUnit = GetWorld()->SpawnActor<AUnit>(EnemyUnitClass, SpawnLocation, SpawnRotation, Params);
        if (EnemyUnit)
        {
            EnemyUnit->CurrentTile = EnemyTiles[EnemyIndex];
            EnemyTiles[EnemyIndex]->OccupiedUnit = EnemyUnit;
            EnemyTiles[EnemyIndex]->bIsOccupied = true;
        }
    }
}

void ABoardManager::HandleTileClicked(ATile* ClickedTile)
{
    if (!ClickedTile) return;

    // ユニット選択
    if (ClickedTile->bIsOccupied)
    {
        SelectedUnit = ClickedTile->OccupiedUnit;
        ClickedTile->SetTileColor(FLinearColor::Yellow);
        return;
    }

    // ユニット移動
    if (SelectedUnit)
    {
        ATile* PreviousTile = SelectedUnit->CurrentTile;
        if (PreviousTile)
        {
            PreviousTile->OccupiedUnit = nullptr;
            PreviousTile->bIsOccupied = false;
            PreviousTile->ResetTileColor();
        }

        SelectedUnit->SetActorLocation(ClickedTile->GetActorLocation() + FVector(0, 0, 150));
        SelectedUnit->CurrentTile = ClickedTile;
        ClickedTile->OccupiedUnit = SelectedUnit;
        ClickedTile->bIsOccupied = true;

        SelectedUnit = nullptr;
    }
}
