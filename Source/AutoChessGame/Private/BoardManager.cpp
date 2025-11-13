#include "BoardManager.h"
#include "Tile.h"
#include "Unit.h"
#include "UGameHUD.h"
#include "PlayerManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

ABoardManager::ABoardManager()
{
    PrimaryActorTick.bCanEverTick = false; // Tick は使わずタイマーでラウンド管理
    SelectedUnit = nullptr;
    CurrentPhase = EGamePhase::Preparation;
}

void ABoardManager::BeginPlay()
{
    Super::BeginPlay();

    GenerateBoard();
    SpawnInitialUnits();

    // HUD作成
    if (HUDClass)
    {
        //UE_LOG(LogTemp, Warning, TEXT("TEXT"));
        HUDInstance = CreateWidget<UUserWidget>(GetWorld(), HUDClass);
        if (HUDInstance)
        {
            HUDInstance->AddToViewport();
        }
    }

    if (PlayerHUDClass)
    {
        PlayerHUDInstance = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);
        if (PlayerHUDInstance)
        {
            PlayerHUDInstance->AddToViewport();
        }
    }

    if (PlayerManagerClass)
    {
        PlayerMangerInstance = GetWorld()->SpawnActor<APlayerManager>(PlayerManagerClass);
       
    }

 


    // 準備フェーズ開始
    StartPreparationPhase();
}

void ABoardManager::GenerateBoard()
{
    if (!TileClass) return;

    FVector Origin = FVector::ZeroVector;
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
                Tile->SetTileColor(FLinearColor(0.2f, 0.4f, 1.f, 1.f));
                Tile->bIsPlayerTile = true;
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

    // プレイヤー：前列2行にユニットを配置
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
    //ショップを開く
    OpenShop();
    CurrentPhase = EGamePhase::Preparation;
    UE_LOG(LogTemp, Warning, TEXT("=== Preparation Phase ==="));

    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit) Unit->bCanDrag = true;
    }

    UpdateHUD();

    // 7秒後に自動で BattlePhase 開始
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

    // ここでラウンド番号をインクリメント
    //CurrentRound++;
    UE_LOG(LogTemp, Warning, TEXT("Battle Phase Started! Round %d"), CurrentRound);

    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit) Unit->bCanDrag = false;
    }

    // BattlePhase タイマー開始
    /*GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this,
        &ABoardManager::ProcessBattleTick,
        TurnInterval,
        true
    );*/
    UpdateHUD();
    //ラウンド開始
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

    // 3秒後に PreparationPhase 再開
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

    // 現在のタイマーを止める
    GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);

    // 既存ユニットを削除
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

    // タイルリセット
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

    // ユニット再生成
    SpawnInitialUnits();

    // Spawn後すぐにドラッグ不可＆位置を強制セット
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

    // ラウンド番号更新
    CurrentRound++;
    UE_LOG(LogTemp, Warning, TEXT("Next Round: %d"), CurrentRound);

    // 準備フェーズへ移行
    StartPreparationPhase();
}



void ABoardManager::StartNextRound()
{
    if (CurrentPhase != EGamePhase::Battle) return;

    // タイマーで繰り返し ProcessBattleTick を呼ぶ
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
            ResultText->SetText(FText::FromString("Result:")); // 初期化やリセット
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
