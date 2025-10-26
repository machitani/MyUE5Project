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

    //ラウンド開始
    CurrentRound = 1;
    GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &ABoardManager::StartNextRound, TurnInterval, false);
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

    // プレイヤー：前列2行にユニットを配置
    for (int32 Row = 0; Row < 2; Row++)
    {
        for (int32 Col = 0; Col < Columns; Col += 2) // 1マスおきに配置
        {
            int32 Index = Row * Columns + Col;
            if (PlayerTiles.IsValidIndex(Index))
            {
                FVector SpawnLocation = PlayerTiles[Index]->GetActorLocation() + FVector(0, 0, 150);
                FRotator SpawnRotation = FRotator(0, 90, 0);
                FActorSpawnParameters Params;
                Params.Owner = this;

                AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(PlayerUnitClass, SpawnLocation, SpawnRotation, Params);
                if (NewUnit)
                {
                    NewUnit->Team = EUnitTeam::Player;
                    NewUnit->CurrentTile = PlayerTiles[Index];
                    PlayerTiles[Index]->bIsOccupied = true;
                    PlayerTiles[Index]->OccupiedUnit = NewUnit;
                    PlayerUnits.Add(NewUnit);
                }
            }
        }
    }

    // 敵：後列2行にユニットを配置
    for (int32 Row = Rows - 2; Row < Rows; Row++)
    {
        for (int32 Col = 0; Col < Columns; Col += 2)
        {
            int32 Index = Row * Columns + Col;
            if (EnemyTiles.IsValidIndex(Index))
            {
                FVector SpawnLocation = EnemyTiles[Index]->GetActorLocation() + FVector(0, 0, 150);
                FRotator SpawnRotation = FRotator(0, 270, 0);
                FActorSpawnParameters Params;
                Params.Owner = this;

                AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(EnemyUnitClass, SpawnLocation, SpawnRotation, Params);
                if (NewUnit)
                {
                    NewUnit->Team = EUnitTeam::Enemy;
                    NewUnit->CurrentTile = EnemyTiles[Index];
                    EnemyTiles[Index]->bIsOccupied = true;
                    EnemyTiles[Index]->OccupiedUnit = NewUnit;
                    EnemyUnits.Add(NewUnit);
                }
            }
        }
    }
}


void ABoardManager::HandleTileClicked(ATile* ClickedTile)
{
    if (!ClickedTile) return;
    ClickedTile->SetTileColor(FLinearColor::Yellow);
}

void ABoardManager::StartNextRound()
{
    
    //プレイヤーターン
    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit && Unit->HP > 0.f)
            Unit->CheckForTarget(TurnInterval);
    }

    //敵ターン
    ProcessEnemyTurn();

    //ゲーム終了判定
    bool bPlayerAlive = false;
    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit && Unit->HP > 0.f)
        {
            bPlayerAlive = true;
        }
    }
    bool bEnemiesAlive = false;
    for (AUnit* Unit : EnemyUnits)
    {
        if (Unit && Unit->HP > 0.f)
        {
            bEnemiesAlive = true;
        }
    }

    if (!bPlayerAlive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Defeat!")); return;
    }
    if (!bEnemiesAlive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Victory!")); return;
    }

    //次ラウンド
    CurrentRound++;
    GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &ABoardManager::StartNextRound, TurnInterval, false);
}

void ABoardManager::ProcessEnemyTurn()
{
    for (AUnit* Unit : EnemyUnits)
    {
        if (Unit && Unit->HP > 0.f)
        {
            Unit->CheckForTarget(TurnInterval);
        }
    }
}

void ABoardManager::StartBattlePhase()
{
    if (CurrentPhase != EGamePhase::Preparation)
        return;

    UE_LOG(LogTemp, Warning, TEXT("Battle Phase Started!"));
    CurrentPhase = EGamePhase::Battle;

    // 戦闘開始処理（ラウンドを進行）
    CurrentRound = 1;
    StartNextRound();
}

void ABoardManager::EndBattlePhase()
{
    if (CurrentPhase != EGamePhase::Battle)
        return;

    UE_LOG(LogTemp, Warning, TEXT("Battle Phase Ended!"));
    CurrentPhase = EGamePhase::Result;

    // 結果処理（あとでUIなどに繋げる）
}
