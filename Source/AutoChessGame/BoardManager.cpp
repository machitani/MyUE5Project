// Fill out your copyright notice in the Description page of Project Settings.

#include "BoardManager.h"
#include "Tile.h"
#include "Unit.h"
#include "Engine/World.h"

// Sets default values
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

    FVector Origin = FVector(0, 0, 0); // 左下基準
    const float BoardGap = 0.f; // プレイヤーと敵の間の空間

    // ===== プレイヤーボード =====
    for (int32 Row = 0; Row < 4; Row++) // 縦4
    {
        for (int32 Col = 0; Col < 7; Col++) // 横7
        {
            FVector SpawnLocation = Origin + FVector(Col * TileSpacing, Row * TileSpacing, 0.f);
            FActorSpawnParameters Params;
            ATile* Tile = GetWorld()->SpawnActor<ATile>(TileClass, SpawnLocation, FRotator::ZeroRotator, Params);
            if (Tile)
            {
                Tile->SetTileColor(FLinearColor(0.2f, 0.4f, 1.0f, 1.0f)); // 青
                PlayerTiles.Add(Tile);
            }
        }
    }

    // ===== 敵ボード =====
    // プレイヤーボードの上に4行分＋隙間をずらして配置
    for (int32 Row = 0; Row < 4; Row++) // 縦4
    {
        for (int32 Col = 0; Col < 7; Col++) // 横7
        {
            FVector SpawnLocation = Origin + FVector(Col * TileSpacing, (Row + 4) * TileSpacing + BoardGap, 0.f);
            FActorSpawnParameters Params;
            ATile* Tile = GetWorld()->SpawnActor<ATile>(TileClass, SpawnLocation, FRotator::ZeroRotator, Params);
            if (Tile)
            {
                Tile->SetTileColor(FLinearColor(1.0f, 0.3f, 0.3f, 1.0f)); // 赤
                EnemyTiles.Add(Tile);
            }
        }
    }
}
void ABoardManager::SpawnInitialUnits()
{
    if (!PlayerUnitClass || !EnemyUnitClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BoardManager: UnitClass not set!"));
        return;
    }

    // 真ん中のタイル（中央）
    int32 CenterX = Columns / 2;
    int32 CenterY = Rows / 2;

    int32 PlayerIndex = CenterY * Columns + CenterX;
    int32 EnemyIndex = CenterY * Columns + CenterX;

    // ===== プレイヤーユニット =====
    if (PlayerTiles.IsValidIndex(PlayerIndex))
    {
        FVector SpawnLocation = PlayerTiles[PlayerIndex]->GetActorLocation() + FVector(0, 0, 200);
        FRotator SpawnRotation = FRotator::ZeroRotator;
        FActorSpawnParameters Params;
        Params.Owner = this;

        AUnit* PlayerUnit = GetWorld()->SpawnActor<AUnit>(PlayerUnitClass, SpawnLocation, SpawnRotation, Params);
        if (PlayerUnit)
        {
            UE_LOG(LogTemp, Log, TEXT("Spawned Player Unit at %s"), *SpawnLocation.ToString());
        }
    }

    // ===== 敵ユニット =====
    if (EnemyTiles.IsValidIndex(EnemyIndex))
    {
        FVector SpawnLocation = EnemyTiles[EnemyIndex]->GetActorLocation() + FVector(0, 0, 150);
        FRotator SpawnRotation = FRotator(0, 180, 0);
        FActorSpawnParameters Params;
        Params.Owner = this;

        AUnit* EnemyUnit = GetWorld()->SpawnActor<AUnit>(EnemyUnitClass, SpawnLocation, SpawnRotation, Params);
        if (EnemyUnit)
        {
            UE_LOG(LogTemp, Log, TEXT("Spawned Enemy Unit at %s"), *SpawnLocation.ToString());
        }
    }
}
