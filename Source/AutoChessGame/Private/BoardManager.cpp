#include "BoardManager.h"
#include "Tile.h"
#include "Unit.h"
#include "Engine/World.h"
#include "TimerManager.h"

ABoardManager::ABoardManager()
{
    PrimaryActorTick.bCanEverTick = false; // Tick �͎g�킸�^�C�}�[�Ń��E���h�Ǘ�
    SelectedUnit = nullptr;
    CurrentPhase = EGamePhase::Preparation;
}

void ABoardManager::BeginPlay()
{
    Super::BeginPlay();

    GenerateBoard();
    SpawnInitialUnits();

    // �����t�F�[�Y�J�n
    StartPreparationPhase();
}

void ABoardManager::GenerateBoard()
{
    if (!TileClass) return;

    FVector Origin = FVector::ZeroVector;
    const float BoardGap = 0.f;

    // �v���C���[�{�[�h
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
                Tile->SetTileColor(FLinearColor(0.2f, 0.4f, 1.f, 1.f));
                Tile->bIsPlayerTile = true;
                PlayerTiles.Add(Tile);
            }
        }
    }

    // �G�{�[�h
    for (int32 Row = 0; Row < Rows; Row++)
    {
        for (int32 Col = 0; Col < Columns; Col++)
        {
            FVector SpawnLocation = Origin + FVector(Col * TileSpacing, (Row + Rows) * TileSpacing + BoardGap, 0.f);
            FActorSpawnParameters Params;

            ATile* Tile = GetWorld()->SpawnActor<ATile>(TileClass, SpawnLocation, FRotator::ZeroRotator, Params);
            if (Tile)
            {
                Tile->SetTileColor(FLinearColor(1.f, 0.3f, 0.3f, 1.f));
                Tile->bIsPlayerTile = false;
                EnemyTiles.Add(Tile);
            }
        }
    }
}

void ABoardManager::SpawnInitialUnits()
{
    if (!PlayerUnitClass || !EnemyUnitClass) return;

    // �v���C���[�F�O��2�s�Ƀ��j�b�g��z�u
    for (int32 Row = 0; Row < 2; Row++)
    {
        for (int32 Col = 0; Col < Columns; Col += 2)
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
                    NewUnit->OwningBoardManager = this;
                    PlayerTiles[Index]->bIsOccupied = true;
                    PlayerTiles[Index]->OccupiedUnit = NewUnit;
                    PlayerUnits.Add(NewUnit);
                }
            }
        }
    }

    // �G�F���2�s�Ƀ��j�b�g��z�u
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
                    NewUnit->OwningBoardManager = this;
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

void ABoardManager::StartPreparationPhase()
{
    CurrentPhase = EGamePhase::Preparation;
    UE_LOG(LogTemp, Warning, TEXT("=== Preparation Phase ==="));

    // ���j�b�g���h���b�O�\��
    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit) Unit->bCanDrag = true;
    }

    // 5�b��Ɏ����Ńo�g���t�F�[�Y�J�n
    GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &ABoardManager::StartBattlePhase, 7.0f, false);
}

void ABoardManager::StartBattlePhase()
{
    if (CurrentPhase != EGamePhase::Preparation) return;

    CurrentPhase = EGamePhase::Battle;
    UE_LOG(LogTemp, Warning, TEXT("Battle Phase Started!"));

    // �v���C���[���j�b�g�̓h���b�O�s��
    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit) Unit->bCanDrag = false;
    }

    // ���E���h�J�n
    CurrentRound = 1;
    StartNextRound();
}

void ABoardManager::EndBattlePhase()
{
    if (CurrentPhase != EGamePhase::Battle) return;

    CurrentPhase = EGamePhase::Result;
    UE_LOG(LogTemp, Warning, TEXT("Battle Phase Ended!"));
}

void ABoardManager::StartResultPhase()
{
    CurrentPhase = EGamePhase::Result;

    // 3�b��Ɏ��̏����t�F�[�Y
    GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &ABoardManager::StartPreparationPhase, 3.0f, false);
}

void ABoardManager::StartNextRound()
{
    if (CurrentPhase != EGamePhase::Battle) return;

    // �^�C�}�[�ŌJ��Ԃ� ProcessBattleTick ���Ă�
    GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &ABoardManager::ProcessBattleTick, TurnInterval, true);
}

void ABoardManager::ProcessBattleTick()
{
    // �v���C���[���j�b�g�s��
    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit && Unit->HP > 0.f)
        {
            //Unit->CheckForTarget(TurnInterval);
        }
    }

    // �G���j�b�g�s��
    for (AUnit* Unit : EnemyUnits)
    {
        if (Unit && Unit->HP > 0.f)
        {
            //Unit->CheckForTarget(TurnInterval);
        }
    }

    // ���s����
    bool bPlayerAlive = false;
    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit && Unit->HP > 0.f)
        {
            bPlayerAlive = true;
            break;
        }
    }

    bool bEnemiesAlive = false;
    for (AUnit* Unit : EnemyUnits)
    {
        if (Unit && Unit->HP > 0.f)
        {
            bEnemiesAlive = true;
            break;
        }
    }

    if (!bPlayerAlive || !bEnemiesAlive)
    {
        GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);

        if (!bPlayerAlive)
        {
            UE_LOG(LogTemp, Warning, TEXT("Defeat!"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Victory!"));
        }
    }
}

void ABoardManager::ProcessEnemyTurn()
{
    for (AUnit* Unit : EnemyUnits)
    {
        if (Unit && Unit->HP > 0.f)
        {
            //Unit->CheckForTarget(TurnInterval);
        }
    }
}
