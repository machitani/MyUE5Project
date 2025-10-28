#include "BoardManager.h"
#include "Tile.h"
#include "Unit.h"
#include "UGameHUD.h"
#include "PlayerManager.h"
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

    // HUD�쐬
    if (HUDClass)
    {
        //UE_LOG(LogTemp, Warning, TEXT("TEXT"));
        HUDInstance = CreateWidget<UUserWidget>(GetWorld(), HUDClass);
        if (HUDInstance)
        {
            HUDInstance->AddToViewport();
        }
    }

    if (PlayerManagerClass)
    {
        PlayerMangerInstance = GetWorld()->SpawnActor<APlayerManager>(PlayerManagerClass);
       
    }

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
    //�V���b�v���J��
    OpenShop();
    CurrentPhase = EGamePhase::Preparation;
    UE_LOG(LogTemp, Warning, TEXT("=== Preparation Phase ==="));

    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit) Unit->bCanDrag = true;
    }

    UpdateHUD();

    // 7�b��Ɏ����� BattlePhase �J�n
    GetWorld()->GetTimerManager().SetTimer(
        PhaseTimerHandle,
        this,
        &ABoardManager::StartBattlePhase,
        7.0f,
        false
    );
}

void ABoardManager::StartBattlePhase()
{
    if (CurrentPhase != EGamePhase::Preparation) return;

    CurrentPhase = EGamePhase::Battle;

    // �����Ń��E���h�ԍ����C���N�������g
    //CurrentRound++;
    UE_LOG(LogTemp, Warning, TEXT("Battle Phase Started! Round %d"), CurrentRound);

    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit) Unit->bCanDrag = false;
    }

    // BattlePhase �^�C�}�[�J�n
    /*GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this,
        &ABoardManager::ProcessBattleTick,
        TurnInterval,
        true
    );*/
    UpdateHUD();
    //���E���h�J�n
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
    UE_LOG(LogTemp, Warning, TEXT("=== Result Phase ==="));

    UpdateHUD();

    // 3�b��� PreparationPhase �ĊJ
    GetWorld()->GetTimerManager().SetTimer(
        PhaseTimerHandle,
        this,
        &ABoardManager::ResetBoardForNextRound,
        3.0f,
        false
    );
}

void ABoardManager::ResetBoardForNextRound()
{
    UE_LOG(LogTemp, Warning, TEXT("== ResetBoardForNextRound() =="));

    // ���݂̃^�C�}�[���~�߂�
    GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);

    // �������j�b�g���폜
    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit) Unit->Destroy();
    }
    PlayerUnits.Empty();

    for (AUnit* Unit : EnemyUnits)
    {
        if (Unit) Unit->Destroy();
    }
    EnemyUnits.Empty();

    // �^�C�����Z�b�g
    for (ATile* Tile : PlayerTiles)
    {
        if (Tile)
        {
            Tile->bIsOccupied = false;
            Tile->OccupiedUnit = nullptr;
            Tile->SetTileColor(FLinearColor(0.2f, 0.4f, 1.f, 1.f));
        }
    }
    for (ATile* Tile : EnemyTiles)
    {
        if (Tile)
        {
            Tile->bIsOccupied = false;
            Tile->OccupiedUnit = nullptr;
            Tile->SetTileColor(FLinearColor(1.f, 0.3f, 0.3f, 1.f));
        }
    }

    // ���j�b�g�Đ���
    SpawnInitialUnits();

    // Spawn�シ���Ƀh���b�O�s���ʒu�������Z�b�g
    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit)
        {
            Unit->bCanDrag = false;
            if (Unit->CurrentTile)
            {
                FVector SpawnLoc = Unit->CurrentTile->GetActorLocation() + FVector(0, 0, 150);
                Unit->SetActorLocation(SpawnLoc);
                Unit->OriginalLocation = SpawnLoc;
            }
        }
    }
    for (AUnit* Unit : EnemyUnits)
    {
        if (Unit)
        {
            Unit->bCanDrag = false;
            if (Unit->CurrentTile)
            {
                FVector SpawnLoc = Unit->CurrentTile->GetActorLocation() + FVector(0, 0, 150);
                Unit->SetActorLocation(SpawnLoc);
                Unit->OriginalLocation = SpawnLoc;
            }
        }
    }

    // ���E���h�ԍ��X�V
    CurrentRound++;
    UE_LOG(LogTemp, Warning, TEXT("Next Round: %d"), CurrentRound);

    // �����t�F�[�Y�ֈڍs
    StartPreparationPhase();
}



void ABoardManager::StartNextRound()
{
    if (CurrentPhase != EGamePhase::Battle) return;

    // �^�C�}�[�ŌJ��Ԃ� ProcessBattleTick ���Ă�
    GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &ABoardManager::ProcessBattleTick, TurnInterval, true);
}

void ABoardManager::ProcessBattleTick()
{
    bool bPlayerAlive = false;
    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit && Unit->HP > 0.f) { bPlayerAlive = true; break; }
    }

    bool bEnemiesAlive = false;
    for (AUnit* Unit : EnemyUnits)
    {
        if (Unit && Unit->HP > 0.f) { bEnemiesAlive = true; break; }
    }

    if (!bPlayerAlive || !bEnemiesAlive)
    {
        GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);

        if (!bPlayerAlive)
        {
            UE_LOG(LogTemp, Warning, TEXT("Defeat!"));
        }
        else
            UE_LOG(LogTemp, Warning, TEXT("Victory!"));

        if (HUDInstance)
        {
            UE_LOG(LogTemp, Warning, TEXT("HUDInstance found"));

            UTextBlock* ResultText = Cast<UTextBlock>(HUDInstance->GetWidgetFromName(TEXT("ResultText")));
            if (ResultText)
            {
                UE_LOG(LogTemp, Warning, TEXT("ResultText found setting Victory/Defeat"));
                if (!bPlayerAlive)
                {
                    ResultText->SetText(FText::FromString("Result:Defeat!"));
                }
                else
                {
                    ResultText->SetText(FText::FromString("Result:Victory!"));
                }
            }
        }
        UpdateHUD();
        //ResetBoardForNextRound();
        StartResultPhase();
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

void ABoardManager::UpdateHUD()
{

    if (!HUDInstance) return;
    //UE_LOG(LogTemp, Warning, TEXT("TEXT"));

    UTextBlock* RoundText = Cast<UTextBlock>(HUDInstance->GetWidgetFromName(TEXT("RoundText")));
    if (RoundText)
    {
        RoundText->SetText(FText::FromString(FString::Printf(TEXT("Round: %d"), CurrentRound)));
    }

    UTextBlock* PhaseText = Cast<UTextBlock>(HUDInstance->GetWidgetFromName(TEXT("PhaseText")));
    if (PhaseText)
    {
        FString PhaseName;
        switch (CurrentPhase)
        {
        case EGamePhase::Preparation: PhaseName = "Preparation"; break;
        case EGamePhase::Battle: PhaseName = "Battle"; break;
        case EGamePhase::Result: PhaseName = "Result"; break;
        }
        PhaseText->SetText(FText::FromString(FString::Printf(TEXT("Phase: %s"), *PhaseName)));
    }

    UTextBlock* ResultText = Cast<UTextBlock>(HUDInstance->GetWidgetFromName(TEXT("ResultText")));
    if (CurrentPhase == EGamePhase::Preparation)
    {
        if (ResultText)
        {
            ResultText->SetText(FText::FromString("Result:")); // �������⃊�Z�b�g
        }
    }
}

void ABoardManager::OpenShop()
{
    if (ShopWidgetClass)
    {
        if (!ShopInstance)
        {
            ShopInstance = CreateWidget<UUserWidget>(GetWorld(), ShopWidgetClass);
            if (ShopInstance)
            {
                ShopInstance->AddToViewport();
            }
        }
    }
}
