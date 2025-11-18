#include "BoardManager.h"
#include "Tile.h"
#include "Unit.h"
#include "UGameHUD.h"
#include "PlayerManager.h"
#include "ShopManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"


ABoardManager::ABoardManager()
{
    PrimaryActorTick.bCanEverTick = false;
    SelectedUnit = nullptr;
    CurrentPhase = EGamePhase::Preparation;
    bRoundEnded = false;
}

void ABoardManager::BeginPlay()
{
    Super::BeginPlay();

    GenerateBoard();
    SpawnInitialUnits();

    // --- HUD作成（1回だけ） ---
    if (HUDClass && !HUDInstance)
    {
        HUDInstance = CreateWidget<UUserWidget>(GetWorld(), HUDClass);
        if (HUDInstance)
        {
            HUDInstance->AddToViewport();
        }
    }

    if (PlayerHUDClass && !PlayerHUDInstance)
    {
        PlayerHUDInstance = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);
        if (PlayerHUDInstance)
        {
            PlayerHUDInstance->AddToViewport();
        }
    }

    // --- レベルに配置済み PlayerManager を取得 ---
    {
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerManager::StaticClass(), Found);

        if (Found.Num() > 0)
        {
            PlayerMangerInstance = Cast<APlayerManager>(Found[0]);
            UE_LOG(LogTemp, Warning, TEXT("PlayerManager found: %s"), *GetNameSafe(PlayerMangerInstance));
        }
        else
        {
            PlayerMangerInstance = nullptr;
            UE_LOG(LogTemp, Error, TEXT("No PlayerManager found in level!"));
        }
    }

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
            ATile* Tile = GetWorld()->SpawnActor<ATile>(TileClass, SpawnLocation, FRotator::ZeroRotator);
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
            ATile* Tile = GetWorld()->SpawnActor<ATile>(TileClass, SpawnLocation, FRotator::ZeroRotator);
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

    // プレイヤー側
    for (int32 Row = 0; Row < 2; Row++)
    {
        for (int32 Col = 0; Col < Columns; Col += 2)
        {
            int32 Index = Row * Columns + Col;
            if (PlayerTiles.IsValidIndex(Index))
            {
                FVector SpawnLocation = PlayerTiles[Index]->GetActorLocation() + FVector(0, 0, 100);
                FActorSpawnParameters Params;
                Params.Owner = this;

                AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(PlayerUnitClass, SpawnLocation, FRotator(0, 0, 0), Params);
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

    // 敵側
    for (int32 Row = Rows - 2; Row < Rows; Row++)
    {
        for (int32 Col = 0; Col < Columns; Col += 2)
        {
            int32 Index = Row * Columns + Col;
            if (EnemyTiles.IsValidIndex(Index))
            {
                FVector SpawnLocation = EnemyTiles[Index]->GetActorLocation() + FVector(0, 0,100);
                FActorSpawnParameters Params;
                Params.Owner = this;

                AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(EnemyUnitClass, SpawnLocation, FRotator(0, 180, 0), Params);
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
    GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);

    CurrentPhase = EGamePhase::Preparation;
    bRoundEnded = false;

    OpenShop();

    UE_LOG(LogTemp, Warning, TEXT("=== Preparation Phase ==="));

    for (AUnit* Unit : PlayerUnits)
    {
        if (!Unit)continue;

        if (Unit->InitialTile)   // ← 保存しておく必要あり
        {
            Unit->SetActorLocation(Unit->InitialTile->GetActorLocation());
            Unit->CurrentTile = Unit->InitialTile;
        }
        Unit->HP = Unit->BaseHP;
        Unit->ReapplayAllItemEffects();

        Unit->bCanDrag = true;
    }

    UpdateHUD();
}

void ABoardManager::StartBattlePhase()
{
    if (CurrentPhase != EGamePhase::Preparation) return;

    CurrentPhase = EGamePhase::Battle;
    bRoundEnded = false;

    UE_LOG(LogTemp, Warning, TEXT("Battle Phase Started! Round %d"), CurrentRound);

    for (AUnit* Unit : PlayerUnits)
    {
        if (Unit)
        {
            Unit->bCanDrag = false;
            Unit->ReapplayAllItemEffects();
        }

        UpdateHUD();
        StartNextRound();
    }
}

void ABoardManager::StartNextRound()
{
    if (CurrentPhase != EGamePhase::Battle) return;

    // ★ 多重 RoundTimer を確実に防ぐ
    GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);

    GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this,
        &ABoardManager::ProcessBattleTick,
        TurnInterval,
        true
    );
}

void ABoardManager::ProcessBattleTick()
{
    // ★ ラウンドは1回しか終了させない
    if (bRoundEnded || CurrentPhase != EGamePhase::Battle)
        return;

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
        bRoundEnded = true;
        GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);

        if (!bPlayerAlive)
        {
            UE_LOG(LogTemp, Warning, TEXT("Defeat!"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Victory!"));

            if (PlayerMangerInstance)
            {
                PlayerMangerInstance->AddExp(2);
                UE_LOG(LogTemp, Warning, TEXT("Round Clear: +2 EXP"));
            }
        }

        if (HUDInstance)
        {
            UTextBlock* ResultText = Cast<UTextBlock>(HUDInstance->GetWidgetFromName(TEXT("ResultText")));
            if (ResultText)
            {
                if (!bPlayerAlive) ResultText->SetText(FText::FromString("Result:Defeat!"));
                else ResultText->SetText(FText::FromString("Result:Victory!"));
            }
        }

        UpdateHUD();
        StartResultPhase();
    }
}

void ABoardManager::EndBattlePhase()
{
    // 今の仕様では Result Phase に移行するだけ
    CurrentPhase = EGamePhase::Result;

    UE_LOG(LogTemp, Warning, TEXT("Battle Phase Ended!"));

    // HUD更新
    UpdateHUD();
}


void ABoardManager::StartResultPhase()
{
    GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);

    CurrentPhase = EGamePhase::Result;
    UE_LOG(LogTemp, Warning, TEXT("=== Result Phase ==="));

    UpdateHUD();

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

    GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);

    bRoundEnded = false;

    // ★ プレイヤーユニット保存して Destroy
    if (PlayerMangerInstance)
    {
        PlayerMangerInstance->SavedUnits.Empty();

        for (AUnit* Unit : PlayerUnits)
        {
            if (!Unit) continue;

            FUnitSaveData Data = Unit->MakeSaveData();
            PlayerMangerInstance->SavedUnits.Add(Data);

            Unit->Destroy();
        }
    }
    PlayerUnits.Empty();

    // ★ 敵ユニットは単純に Destroy
    for (AUnit* Unit : EnemyUnits)
    {
        if (Unit) Unit->Destroy();
    }
    EnemyUnits.Empty();

    // ★ タイルリセット
    for (ATile* Tile : PlayerTiles)
    {
        Tile->bIsOccupied = false;
        Tile->OccupiedUnit = nullptr;
        Tile->SetTileColor(FLinearColor(0.2f, 0.4f, 1.f, 1.f));
    }
    for (ATile* Tile : EnemyTiles)
    {
        Tile->bIsOccupied = false;
        Tile->OccupiedUnit = nullptr;
        Tile->SetTileColor(FLinearColor(1.f, 0.3f, 0.3f, 1.f));
    }

    SpawnPlayerUnitsFromSaveData();
    SpawnEnemyUnits();

    CurrentRound++;
    UE_LOG(LogTemp, Warning, TEXT("Next Round: %d"), CurrentRound);

    StartPreparationPhase();
}

void ABoardManager::UpdateHUD()
{
    if (!HUDInstance) return;

    UTextBlock* RoundText = Cast<UTextBlock>(HUDInstance->GetWidgetFromName(TEXT("RoundText")));
    if (RoundText)
        RoundText->SetText(FText::FromString(FString::Printf(TEXT("Round: %d"), CurrentRound)));

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
            ResultText->SetText(FText::FromString("Result:"));
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

void ABoardManager::ProcessEnemyTurn()
{
    for (AUnit* Unit : EnemyUnits)
        if (Unit && Unit->HP > 0.f)
        {
            // ここに敵AIを書く
        }
}

void ABoardManager::MoveUnitToTile(AUnit* Unit, ATile* NewTile)
{
    if (!Unit || !NewTile) return;

    ATile* OldTile = Unit->CurrentTile;
    if (OldTile)
    {
        OldTile->bIsOccupied = false;
        OldTile->OccupiedUnit = nullptr;
    }

    NewTile->bIsOccupied = true;
    NewTile->OccupiedUnit = Unit;
    Unit->CurrentTile = NewTile;

    FVector NewLocation = NewTile->GetActorLocation() + FVector(0, 0, 150);
    Unit->SetActorLocation(NewLocation);

    Unit->OriginalLocation = NewLocation;

    UE_LOG(LogTemp, Warning, TEXT("[MoveUnitToTile] Unit moved to %s"), *NewTile->GetName());
}

ATile* ABoardManager::GetTileUnderLocation(const FVector& Location)
{
    for (ATile* Tile : PlayerTiles)
    {
        FVector TileLoc = Tile->GetActorLocation();

        // ★ ここを広くする（2Dで距離判定）
        if (FVector::Dist2D(Location, TileLoc) < 250.f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Tile FOUND: %s"), *Tile->GetName());
            return Tile;
        }
    }
    return nullptr;
}

void ABoardManager::SpawnPlayerUnitsFromSaveData()
{
    if (!PlayerMangerInstance) return;

    PlayerUnits.Empty();

    for (const FUnitSaveData& Data : PlayerMangerInstance->SavedUnits)
    {
        AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(PlayerUnitClass);
        if (!NewUnit) continue;

        // セーブデータ適用
        NewUnit->ApplySaveData(Data);

        int32 TileIndex = Data.SavedTileIndex;
        if (!PlayerTiles.IsValidIndex(TileIndex))
            {
            continue; // 異常値ならスキップ
            }

         ATile * Tile = PlayerTiles[TileIndex];
        FVector SpawnLoc = Tile->GetActorLocation() + FVector(0, 0, 150);

        NewUnit->SetActorLocation(SpawnLoc);
        NewUnit->OriginalLocation = SpawnLoc;
        NewUnit->CurrentTile = Tile;

        Tile->OccupiedUnit = NewUnit;
        Tile->bIsOccupied = true;

        NewUnit->OwningBoardManager = this;

        PlayerUnits.Add(NewUnit);

        //TileIndex++;
    }
}

void ABoardManager::SpawnEnemyUnits()
{
    if (!EnemyUnitClass) return;

    EnemyUnits.Empty();

    for (int32 Row = Rows - 2; Row < Rows; Row++)
    {
        for (int32 Col = 0; Col < Columns; Col += 2)
        {
            int32 Index = Row * Columns + Col;
            if (!EnemyTiles.IsValidIndex(Index)) continue;

            FVector SpawnLocation = EnemyTiles[Index]->GetActorLocation() + FVector(0, 0, 150);

            AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(
                EnemyUnitClass,
                SpawnLocation,
                FRotator(0, 180, 0)
            );

            if (!NewUnit) continue;

            NewUnit->Team = EUnitTeam::Enemy;
            NewUnit->CurrentTile = EnemyTiles[Index];
            NewUnit->OwningBoardManager = this;

            EnemyTiles[Index]->bIsOccupied = true;
            EnemyTiles[Index]->OccupiedUnit = NewUnit;

            EnemyUnits.Add(NewUnit);
        }
    }
}
