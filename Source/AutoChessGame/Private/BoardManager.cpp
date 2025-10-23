#include "BoardManager.h"
#include "Tile.h"
#include "Unit.h"
#include "Engine/World.h"

ABoardManager::ABoardManager()
{
    PrimaryActorTick.bCanEverTick = false;
    SelectedUnit = nullptr;
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

    FVector Origin = FVector(0, 0, 0);
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
                Tile->BoardManagerRef = this;
                Tile->SetTileColor(FLinearColor(0.2f, 0.4f, 1.f, 1.f)); // 青
                Tile->bIsPlayerTile = true;  //プレイヤータイル
                PlayerTiles.Add(Tile);

                UE_LOG(LogTemp, Warning, TEXT("Spawned Player Tile: %s, BoardManagerRef = %s"),
                    *Tile->GetName(),
                    Tile->BoardManagerRef ? *Tile->BoardManagerRef->GetName() : TEXT("None"));
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
                Tile->SetTileColor(FLinearColor(1.f, 0.3f, 0.3f, 1.f)); // 赤
                Tile->bIsPlayerTile = false; //敵タイル
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
        FRotator SpawnRotation = FRotator(0, 90, 0);
        FActorSpawnParameters Params;
        Params.Owner = this;

       AUnit*SpawnedUnit= GetWorld()->SpawnActor<AUnit>(PlayerUnitClass, SpawnLocation, SpawnRotation, Params);
       if (SpawnedUnit)
       {
           ATile* TargetTile = PlayerTiles[PlayerIndex];   // スポーンしたタイル
           SpawnedUnit->CurrentTile = TargetTile;         // CurrentTile を設定
           TargetTile->OccupiedUnit = SpawnedUnit;        // タイル側も設定
           TargetTile->bIsOccupied = true;                // タイル占有フラグ
           SpawnedUnit->OwningBoardManager = this;        // BoardManager参照
           PlayerUnits.Add(SpawnedUnit);                  // 配列に追加
       }

    }

    if (EnemyTiles.IsValidIndex(EnemyIndex))
    {
        FVector SpawnLocation = EnemyTiles[EnemyIndex]->GetActorLocation() + FVector(0, 0, 150);
        FRotator SpawnRotation = FRotator(0, 270, 0);
        FActorSpawnParameters Params;
        Params.Owner = this;

        GetWorld()->SpawnActor<AUnit>(EnemyUnitClass, SpawnLocation, SpawnRotation, Params);

    }
}

void ABoardManager::HandleTileClicked(ATile* ClickedTile)
{
    if (!ClickedTile) return;
    ClickedTile->SetTileColor(FLinearColor::Yellow);
}
