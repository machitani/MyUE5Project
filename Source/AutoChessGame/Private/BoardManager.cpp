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
    if (!PlayerKnightClass || !PlayerWizardClass || !PlayerArcherClass)
        return;

    PlayerUnits.Empty();

    // ★ Knight
    if (PlayerTiles.IsValidIndex(0))
    {
        FVector SpawnLocation = PlayerTiles[0]->GetActorLocation() + FVector(0, 0, 100);

        AUnit* NewKnight = GetWorld()->SpawnActor<AUnit>(PlayerKnightClass, SpawnLocation, FRotator::ZeroRotator);
        if (NewKnight)
        {
            NewKnight->Team = EUnitTeam::Player;
            NewKnight->CurrentTile = PlayerTiles[0];
            NewKnight->OwningBoardManager = this;

            PlayerTiles[0]->bIsOccupied = true;
            PlayerTiles[0]->OccupiedUnit = NewKnight;

            PlayerUnits.Add(NewKnight);
        }
    }

    // ★ Wizard
    if (PlayerTiles.IsValidIndex(2))
    {
        FVector SpawnLocation = PlayerTiles[2]->GetActorLocation() + FVector(0, 0, 100);

        AUnit* NewWizard = GetWorld()->SpawnActor<AUnit>(PlayerWizardClass, SpawnLocation, FRotator::ZeroRotator);
        if (NewWizard)
        {
            NewWizard->Team = EUnitTeam::Player;
            NewWizard->CurrentTile = PlayerTiles[2];
            NewWizard->OwningBoardManager = this;

            PlayerTiles[2]->bIsOccupied = true;
            PlayerTiles[2]->OccupiedUnit = NewWizard;

            PlayerUnits.Add(NewWizard);
        }
    }

    // ★ Archer
    if (PlayerTiles.IsValidIndex(4))
    {
        FVector SpawnLocation = PlayerTiles[4]->GetActorLocation() + FVector(0, 0, 100);

        AUnit* NewArcher = GetWorld()->SpawnActor<AUnit>(PlayerArcherClass, SpawnLocation, FRotator::ZeroRotator);
        if (NewArcher)
        {
            NewArcher->Team = EUnitTeam::Player;
            NewArcher->CurrentTile = PlayerTiles[4];
            NewArcher->OwningBoardManager = this;

            PlayerTiles[4]->bIsOccupied = true;
            PlayerTiles[4]->OccupiedUnit = NewArcher;

            PlayerUnits.Add(NewArcher);
        }
    }

    // ★ 敵側は今まで通り2体
    EnemyUnits.Empty();
    int32 EnemySpawnCount = 2;
    int32 EnemySpawned = 0;

    int32 StartRow = Rows - 1;

    for (int32 Col = 0; Col < Columns && EnemySpawned < EnemySpawnCount; Col += 2)
    {
        int32 TileIndex = StartRow * Columns + Col;

        if (EnemyTiles.IsValidIndex(TileIndex))
        {
            FVector SpawnLocation = EnemyTiles[TileIndex]->GetActorLocation() + FVector(0, 0, 100);

            AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(
                EnemyUnitClass,
                SpawnLocation,
                FRotator(0, 180, 0)
            );

            if (NewUnit)
            {
                NewUnit->Team = EUnitTeam::Enemy;
                NewUnit->CurrentTile = EnemyTiles[TileIndex];
                NewUnit->OwningBoardManager = this;

                EnemyTiles[TileIndex]->bIsOccupied = true;
                EnemyTiles[TileIndex]->OccupiedUnit = NewUnit;

                EnemyUnits.Add(NewUnit);
                EnemySpawned++;
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
            if (ShopManagerRef)
            {
                ShopManagerRef->RoundClearGold();
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

    // 元のタイルを空にする
    if (Unit->CurrentTile && Unit->CurrentTile != NewTile)
    {
        Unit->CurrentTile->bIsOccupied = false;
        Unit->CurrentTile->OccupiedUnit = nullptr;
    }

    // 新しいタイルを占有
    NewTile->bIsOccupied = true;
    NewTile->OccupiedUnit = Unit;
    Unit->CurrentTile = NewTile;

    FVector Center = NewTile->GetTileCenterWorld();

    // ユニットの高さ分だけ浮かせる（ここは好みで調整）
    const float UnitHeightOffset = 50.0f;
    Center.Z += UnitHeightOffset;

    Unit->SetActorLocation(Center);
    Unit->OriginalLocation = Center;

    UE_LOG(LogTemp, Warning,
        TEXT("[MoveUnitToTile] %s -> %s pos=%s"),
        *Unit->GetName(), *NewTile->GetName(), *Center.ToString());
}



ATile* ABoardManager::GetTileUnderLocation(const FVector& Location)
{
    ATile* BestTile = nullptr;
    float  BestDist = TNumericLimits<float>::Max();

    for (ATile* Tile : PlayerTiles)
    {
        if (!Tile) continue;

        FVector Center = Tile->GetTileCenterWorld();
        float   Radius = Tile->GetTileSnapRadius();

        float Dist = FVector::Dist2D(Location, Center);

        // タイルの円の中かつ一番近いものを採用
        if (Dist < Radius && Dist < BestDist)
        {
            BestDist = Dist;
            BestTile = Tile;
        }
    }

    return BestTile; // 見つからなければ nullptr
}

void ABoardManager::SpawnPlayerUnitsFromSaveData()
{
    if (!PlayerMangerInstance) return;

    PlayerUnits.Empty();

    for (const FUnitSaveData& Data : PlayerMangerInstance->SavedUnits)
    {
        // UnitID からクラスを決定
        TSubclassOf<AUnit> UnitClass = GetPlayerUnitClassByID(Data.UnitID);
        if (!UnitClass) continue;

        AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(UnitClass);
        if (!NewUnit) continue;

        // セーブデータ適用
        NewUnit->ApplySaveData(Data);

        int32 TileIndex = Data.SavedTileIndex;
        if (!PlayerTiles.IsValidIndex(TileIndex))
        {
            continue; // 異常値ならスキップ
        }

        ATile* Tile = PlayerTiles[TileIndex];
        FVector SpawnLoc = Tile->GetActorLocation() + FVector(0, 0, 100);

        NewUnit->SetActorLocation(SpawnLoc);
        NewUnit->OriginalLocation = SpawnLoc;
        NewUnit->CurrentTile = Tile;

        Tile->OccupiedUnit = NewUnit;
        Tile->bIsOccupied = true;

        NewUnit->OwningBoardManager = this;

        PlayerUnits.Add(NewUnit);
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

            FVector SpawnLocation = EnemyTiles[Index]->GetActorLocation() + FVector(0, 0, 100);

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

int32 ABoardManager::GetDeployedPlayerUnitCount() const
{
    int32 Count = 0;

    for (AUnit* Unit : PlayerUnits)
    {
        if (!Unit)continue;

        if (Unit->HP > 0.f &&
            Unit->CurrentTile &&
            Unit->CurrentTile->bIsPlayerTile)
        {
            Count++;
        }
    }

    return Count;
}

TSubclassOf<AUnit> ABoardManager::GetPlayerUnitClassByID(FName UnitID) const
{
    if (UnitID == FName("Knight"))
    {
        return PlayerKnightClass;
    }
    else if (UnitID == FName("Wizard"))
    {
        return PlayerWizardClass;
    }
    else if (UnitID == FName("Archer"))
    {
        return PlayerArcherClass;
    }

    // どれにも当てはまらない場合の保険（適当にKnight返すなど）
    return PlayerKnightClass;
}
