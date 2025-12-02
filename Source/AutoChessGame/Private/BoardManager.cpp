#include "BoardManager.h"
#include "Tile.h"
#include "Unit.h"
#include "UGameHUD.h"
#include "PlayerManager.h"
#include "ShopManager.h"
#include "PlayerHUD.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


ABoardManager::ABoardManager()
{
    PrimaryActorTick.bCanEverTick = false;
    SelectedUnit = nullptr;
    CurrentPhase = EGamePhase::Preparation;
    bRoundEnded = false;
    
    CurrentStageIndex = 1;
    CurrentWaveIndex = 1;

    bIsGameClear = false;
    bIsGameOver = false;
    bLastRoundWasVictory = false;

    ItemUnit = nullptr;
}

TArray<FName> ABoardManager::GenerateRewardUnitCandidates(int32 Num)const 
{
    // まず RewardUnitIDList をコピー
    TArray<FName> Available = RewardUnitIDList;

    // ★ すでに持っているユニットを除外
    if (PlayerManagerInstance)
    {
        for (const FName& OwnedID : PlayerManagerInstance->OwnedUnitIDs)
        {
            Available.Remove(OwnedID);
        }
    }

    TArray<FName> Result;

    // 候補ゼロならそのまま返す
    if (Available.Num() == 0)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("GenerateRewardUnitCandidates: No available units (all owned)."));
        return Result;
    }

    // ★ 被りなしでランダムに Num 個まで取り出す
    while (Available.Num() > 0 && Result.Num() < Num)
    {
        int32 Index = FMath::RandRange(0, Available.Num() - 1);
        Result.Add(Available[Index]);

        // 同じIDがまた出ないように候補側から削除
        Available.RemoveAtSwap(Index);
    }

    UE_LOG(LogTemp, Warning,
        TEXT("GenerateRewardUnitCandidates: %d candidates generated."),
        Result.Num());

    return Result;
}

FEnemyWaveData* ABoardManager::GetCurrentWaveData()
{
    if (EnemyWaves.Num() == 0)
    {
        return nullptr;
    }

    FEnemyWaveData* Found = EnemyWaves.FindByPredicate(
        [this](const FEnemyWaveData& Data)
        {
            return Data.StageIndex == CurrentStageIndex
                && Data.WaveIndex == CurrentWaveIndex;
        }
    );

    if (Found)
    {
        return Found;
    }

    // ★ 対応するWaveが無い → 全Waveクリア → ゲームクリア
    UE_LOG(LogTemp, Warning,
        TEXT("GetCurrentWaveData: No data for Stage=%d Wave=%d. GameClear!"),
        CurrentStageIndex, CurrentWaveIndex);

    bIsGameClear = true;
    return nullptr;
}
void ABoardManager::ReviveAllEnemiesOnDefeat()
{
    for (AUnit* Unit : EnemyUnits)
    {
        if (!Unit) continue;

        Unit->HP = Unit->BaseHP;
        Unit->bIsDead = false;
        Unit->bIsAttacking = false;
        Unit->bIsMoving = false;

        if (Unit->UnitMesh)
        {
            Unit->UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }

        if (Unit->CurrentTile)
        {
            FVector Center = Unit->CurrentTile->GetTileCenterWorld();
            Center.Z += 50.f;
            Unit->SetActorLocation(Center);
            Unit->OriginalLocation = Center;
        }
    }
}

void ABoardManager::HandleGameOver()
{
    bIsGameOver = true;
    CurrentPhase = EGamePhase::Result;

    UE_LOG(LogTemp, Warning, TEXT("=== GAME OVER ==="));

    if (HUDInstance)
    {
        if (UTextBlock* ResultText = Cast<UTextBlock>(HUDInstance->GetWidgetFromName(TEXT("ResultText"))))
        {
            ResultText->SetText(FText::FromString(TEXT("Result: GAME OVER")));
        }
    }
}

void ABoardManager::HandleGameClear()
{
    bIsGameClear = true;
    CurrentPhase = EGamePhase::Result;

    UE_LOG(LogTemp, Warning, TEXT("=== GAME CLEAR! ==="));

    if (HUDInstance)
    {
        if (UTextBlock* ResultText = Cast<UTextBlock>(HUDInstance->GetWidgetFromName(TEXT("ResultText"))))
        {
            ResultText->SetText(FText::FromString(TEXT("Result: GAME CLEAR")));
        }
    }
}

void ABoardManager::HandleDefeat()
{
    UE_LOG(LogTemp, Warning, TEXT("HandleDefeat"));

    if (!PlayerManagerInstance)
    {
        // 敗北だけ伝えて、そのままリザルトへ
        StartResultPhase();
        return;
    }

    // ライフを1減らす
    PlayerManagerInstance->PlayerLife--;
    UE_LOG(LogTemp, Warning, TEXT("PlayerLife now: %d"), PlayerManagerInstance->PlayerLife);

    UpdateHUD();

    if (PlayerManagerInstance->PlayerLife <= 0)
    {
        // ライフ0 → ゲームオーバー
        HandleGameOver();
    }
    else
    {
        // まだライフ残ってる → 通常通り ResultPhase へ
        StartResultPhase();
    }
}


void ABoardManager::BeginPlay()
{
    Super::BeginPlay();

    // 1) まずボード生成
    GenerateBoard();

    // 2) PlayerManager を取得（BoardManagerRef を渡す）
    {
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerManager::StaticClass(), Found);

        if (Found.Num() > 0)
        {
            PlayerManagerInstance = Cast<APlayerManager>(Found[0]);
            UE_LOG(LogTemp, Warning, TEXT("PlayerManager found: %s"), *GetNameSafe(PlayerManagerInstance));

            PlayerManagerInstance->BoardManagerRef = this;
        }
        else
        {
            PlayerManagerInstance = nullptr;
            UE_LOG(LogTemp, Error, TEXT("No PlayerManager found in level!"));
        }
    }

    // ★ 3) 初期プレイヤーユニットをスポーン（前と同じ感じ）
    SpawnInitialUnits();   // ← ここで Knight / Archer を出す

    // ★ 4) EnemyWaves から敵ユニットをスポーン
    SpawnEnemyUnits();     // ← 1ラウンド目の敵も Wave データから出す

    // 5) HUD作成
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

    // 6) ShopManager を取得して PlayerManager を渡す
    {
        TArray<AActor*> FoundShop;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShopManager::StaticClass(), FoundShop);

        if (FoundShop.Num() > 0)
        {
            ShopManagerRef = Cast<AShopManager>(FoundShop[0]);
            UE_LOG(LogTemp, Warning, TEXT("ShopManager found: %s"), *GetNameSafe(ShopManagerRef));

            if (ShopManagerRef && PlayerManagerInstance)
            {
                ShopManagerRef->PlayerManagerRef = PlayerManagerInstance;
                UE_LOG(LogTemp, Warning, TEXT("ShopManagerRef->PlayerManagerRef set."));
            }
        }
        else
        {
            ShopManagerRef = nullptr;
            UE_LOG(LogTemp, Error, TEXT("No ShopManager found in level!"));
        }
    }

    // 7) 準備フェーズスタート
    StartPreparationPhase();
}

void ABoardManager::InitializePlayerSavedUnitsIfEmpty()
{
    if (!PlayerManagerInstance)
    {
        UE_LOG(LogTemp, Error,
            TEXT("InitializePlayerSavedUnitsIfEmpty: PlayerManagerInstance is NULL"));
        return;
    }

    // すでにセーブデータがあれば「続きから」とみなして何もしない
    if (PlayerManagerInstance->SavedUnits.Num() > 0)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("InitializePlayerSavedUnitsIfEmpty: SavedUnits already exists. Skip seeding initial units."));
        return;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("InitializePlayerSavedUnitsIfEmpty: Seed initial Knight & Archer."));

    // ★ Knight
    {
        FUnitSaveData KnightData;
        KnightData.UnitID = FName("Knight");
        KnightData.SavedTileIndex = 1;   // PlayerTiles[1] に出す前提（GenerateBoard で埋まっている）

        PlayerManagerInstance->SavedUnits.Add(KnightData);
        PlayerManagerInstance->RegisterOwnedUnit(KnightData.UnitID);
    }

    // ★ Archer
    {
        FUnitSaveData ArcherData;
        ArcherData.UnitID = FName("Archer");
        ArcherData.SavedTileIndex = 5;   // PlayerTiles[5]

        PlayerManagerInstance->SavedUnits.Add(ArcherData);
        PlayerManagerInstance->RegisterOwnedUnit(ArcherData.UnitID);
    }
}


void ABoardManager::SetItemTargetUnit(AUnit* NewUnit)
{
    if (NewUnit)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[BoardManager] SetItemTargetUnit: %s"),
            *NewUnit->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[BoardManager] SetItemTargetUnit: nullptr"));
    }

    ItemUnit = NewUnit;
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
    if (PlayerTiles.IsValidIndex(1))
    {
        FVector SpawnLocation = PlayerTiles[1]->GetActorLocation() + FVector(0, 0, 100);

        AUnit* NewKnight = GetWorld()->SpawnActor<AUnit>(PlayerKnightClass, SpawnLocation, FRotator::ZeroRotator);
        if (NewKnight)
        {
            NewKnight->Team = EUnitTeam::Player;
            NewKnight->CurrentTile = PlayerTiles[1];
            NewKnight->OwningBoardManager = this;
            MoveUnitToTile(NewKnight, PlayerTiles[1]);
            NewKnight->UnitID = FName("Knight");

            PlayerTiles[1]->bIsOccupied = true;
            PlayerTiles[1]->OccupiedUnit = NewKnight;

            PlayerUnits.Add(NewKnight);

            if (PlayerManagerInstance)
            {
                PlayerManagerInstance->RegisterOwnedUnit(NewKnight->UnitID);
            }
        }
    }

    // ★ Archer
    if (PlayerTiles.IsValidIndex(5))
    {
        FVector SpawnLocation = PlayerTiles[5]->GetActorLocation() + FVector(0, 0, 100);

        AUnit* NewArcher = GetWorld()->SpawnActor<AUnit>(PlayerArcherClass, SpawnLocation, FRotator::ZeroRotator);
        if (NewArcher)
        {
            NewArcher->Team = EUnitTeam::Player;
            NewArcher->CurrentTile = PlayerTiles[5];
            NewArcher->OwningBoardManager = this;
            MoveUnitToTile(NewArcher, PlayerTiles[5]);
            NewArcher->UnitID = FName("Archer");
            PlayerTiles[5]->bIsOccupied = true;
            PlayerTiles[5]->OccupiedUnit = NewArcher;

            PlayerUnits.Add(NewArcher);

            if (PlayerManagerInstance)
            {
                PlayerManagerInstance->RegisterOwnedUnit(NewArcher->UnitID);
            }
        }
    }

    // ★ 敵スポーン部分（EnemyUnitClass で 2体出してた for文）は丸ごと削除してOK
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

    UE_LOG(LogTemp, Warning,
        TEXT("[Prep] StartPreparationPhase: PlayerUnits=%d"),
        PlayerUnits.Num());

    for (AUnit* Unit : PlayerUnits)
    {
        if (!Unit) continue;

        // 位置はいじらないでそのまま
        UE_LOG(LogTemp, Warning,
            TEXT("[Prep] Unit=%s Loc=%s Tile=%s"),
            *Unit->GetName(),
            *Unit->GetActorLocation().ToString(),
            Unit->CurrentTile ? *Unit->CurrentTile->GetName() : TEXT("NULL"));

        // HP・アイテム・ドラッグ許可だけリセット
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
    }
        UpdateHUD();
        StartNextRound();
    
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
    if (bRoundEnded || CurrentPhase != EGamePhase::Battle || bIsGameOver || bIsGameClear)
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

            // ★ 敗北したラウンド
            bLastRoundWasVictory = false;

            // ラウンド敗北 → ライフ減少＆GameOver判定
            HandleDefeat();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Victory!"));

            // ★ 勝利したラウンド
            bLastRoundWasVictory = true;

            if (PlayerManagerInstance)
            {
                PlayerManagerInstance->AddExp(2);
                UE_LOG(LogTemp, Warning, TEXT("Round Clear: +2 EXP"));
            }
            if (ShopManagerRef)
            {
                ShopManagerRef->RoundClearGold();
                UE_LOG(LogTemp, Warning, TEXT("CLEAR GOLD"));
            }

            // 勝利時は通常通り ResultPhase → 次のWaveへ
            StartResultPhase();
        }
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

    // ★ もうゲーム終了していたら何もしない
    if (bIsGameOver || bIsGameClear)
    {
        GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);
        UE_LOG(LogTemp, Warning, TEXT("Game finished. Skip ResetBoardForNextRound."));
        return;
    }

    // タイマーは一旦全部止める
    GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(PhaseTimerHandle);

    bRoundEnded = false;

    // === ここが前と同じ「リセット処理」 ===

    // ★ プレイヤーユニット保存して Destroy
    if (PlayerManagerInstance)
    {
        PlayerManagerInstance->SavedUnits.Empty();

        for (AUnit* Unit : PlayerUnits)
        {
            if (!Unit) continue;

            FUnitSaveData Data = Unit->MakeSaveData();
            PlayerManagerInstance->SavedUnits.Add(Data);

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
        if (!Tile) continue;
        Tile->bIsOccupied = false;
        Tile->OccupiedUnit = nullptr;
        Tile->SetTileColor(FLinearColor(0.2f, 0.4f, 1.f, 1.f));
    }
    for (ATile* Tile : EnemyTiles)
    {
        if (!Tile) continue;
        Tile->bIsOccupied = false;
        Tile->OccupiedUnit = nullptr;
        Tile->SetTileColor(FLinearColor(1.f, 0.3f, 0.3f, 1.f));
    }

    // === ここから Wave 関連 ===

    // 勝利したラウンドから呼ばれている前提なら、とりあえず Wave を1つ進める
    if (bLastRoundWasVictory)
    {
        CurrentWaveIndex++;
        CurrentRound++;
    }
    else
    {
        // 敗北 → 同じ Wave をリトライ
        UE_LOG(LogTemp, Warning,
            TEXT("Retry same Wave: Stage=%d Wave=%d"),
            CurrentStageIndex, CurrentWaveIndex);
    }

    // 次のWaveデータがあるか確認
    FEnemyWaveData* NextWave = GetCurrentWaveData();
    if (!NextWave)
    {
        // GetCurrentWaveData 内で bIsGameClear = true; しているならここで終了
        HandleGameClear();
        return;
    }

    if (ShopManagerRef)
    {
        ShopManagerRef->RerollShop(4); // スロット数に合わせて
    }


    // ★ セーブデータからプレイヤーユニットを復元
    SpawnPlayerUnitsFromSaveData();

    // ★ Waveデータを使って敵ユニットを生成
    SpawnEnemyUnits();
    StartPreparationPhase();
}


void ABoardManager::UpdateHUD()
{
    if (!HUDInstance) return;

    // ラウンド番号だけをバッジに表示
    if (UTextBlock* RoundNumText = Cast<UTextBlock>(HUDInstance->GetWidgetFromName(TEXT("RoundNumberText"))))
    {
        RoundNumText->SetText(
            FText::FromString(FString::Printf(TEXT("%d"), CurrentRound))
        );
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
            ResultText->SetText(FText::FromString("Result:"));
    }

    if (PlayerManagerInstance)
    {
        int32 Life = PlayerManagerInstance->PlayerLife;

        UImage* Heart1 = Cast<UImage>(HUDInstance->GetWidgetFromName(TEXT("Heart1")));
        UImage* Heart2 = Cast<UImage>(HUDInstance->GetWidgetFromName(TEXT("Heart2")));
        UImage* Heart3 = Cast<UImage>(HUDInstance->GetWidgetFromName(TEXT("Heart3")));

        auto SetHeartVisible = [](UImage* Heart, bool bVisible)
            {
                if (!Heart) return;
                Heart->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
            };

        SetHeartVisible(Heart1, Life >= 1);
        SetHeartVisible(Heart2, Life >= 2);
        SetHeartVisible(Heart3, Life >= 3);
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

    UE_LOG(LogTemp, Warning,
        TEXT("[BoardManager] MoveUnitToTile: %s -> %s"),
        *Unit->GetName(),
        *NewTile->GetName());

    ATile* OldTile = Unit->CurrentTile;

    // 他人がいるマスには置かない
    if (NewTile->bIsOccupied && NewTile->OccupiedUnit != Unit)
    {
        Unit->SetActorLocation(Unit->OriginalLocation);
        return;
    }

    if (OldTile && OldTile != NewTile)
    {
        OldTile->bIsOccupied = false;
        OldTile->OccupiedUnit = nullptr;
    }

    NewTile->bIsOccupied = true;
    NewTile->OccupiedUnit = Unit;
    Unit->CurrentTile = NewTile;

    FVector Center = NewTile->GetTileCenterWorld();
    Center.Z += 50.f;

    Unit->SetActorLocation(Center);
    Unit->OriginalLocation = Center;

    NewTile->SetTileHighlight(false);
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
    if (!PlayerManagerInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnPlayerUnitsFromSaveData: PlayerManagerInstance is NULL"));
        return;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("SpawnPlayerUnitsFromSaveData: SavedUnits=%d"),
        PlayerManagerInstance->SavedUnits.Num());

    PlayerUnits.Empty();

    for (const FUnitSaveData& Data : PlayerManagerInstance->SavedUnits)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Load] UnitID=%s TileIndex=%d"),
            *Data.UnitID.ToString(),
            Data.SavedTileIndex);

        TSubclassOf<AUnit> UnitClass = GetPlayerUnitClassByID(Data.UnitID);
        if (!UnitClass)
        {
            UE_LOG(LogTemp, Error, TEXT("  -> No UnitClass for ID=%s"), *Data.UnitID.ToString());
            continue;
        }

        int32 TileIndex = Data.SavedTileIndex;
        if (!PlayerTiles.IsValidIndex(TileIndex))
        {
            UE_LOG(LogTemp, Error, TEXT("  -> Invalid TileIndex=%d"), TileIndex);
            continue;
        }

        ATile* Tile = PlayerTiles[TileIndex];
        FVector SpawnLoc = Tile->GetActorLocation() + FVector(0, 0, 100);

        AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(UnitClass, SpawnLoc, FRotator::ZeroRotator);
        if (!NewUnit)
        {
            UE_LOG(LogTemp, Error, TEXT("  -> Failed SpawnActor for ID=%s"), *Data.UnitID.ToString());
            continue;
        }

        // セーブデータ適用
        NewUnit->ApplySaveData(Data);

        // 管理系
        NewUnit->Team = EUnitTeam::Player;
        NewUnit->OwningBoardManager = this;
        NewUnit->UnitID = Data.UnitID;

        // 初期タイルを設定
        NewUnit->InitialTile = Tile;

        // ここでタイルに正式に配置（OriginalLocation や Occupied も一括でやる）
        MoveUnitToTile(NewUnit, Tile);

        // 戦闘フラグ系リセット（必要なら）
        NewUnit->bIsAttacking = false;
        NewUnit->bIsDead = false;
        NewUnit->bCanDrag = true;

        PlayerUnits.Add(NewUnit);

        UE_LOG(LogTemp, Warning,
            TEXT("  -> Spawned %s at TileIndex=%d (%s)"),
            *Data.UnitID.ToString(),
            TileIndex,
            *Tile->GetName());

        if (PlayerManagerInstance)
        {
            PlayerManagerInstance->RegisterOwnedUnit(NewUnit->UnitID);
        }
    }
}


void ABoardManager::SpawnEnemyUnits()
{
    EnemyUnits.Empty();

    // ★ 今の Stage/Wave に対応するデータを取得
    FEnemyWaveData* WaveData = GetCurrentWaveData();
    if (!WaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnEnemyUnits: No WaveData. No enemies spawned."));
        return;
    }

    // ★ 敵を置けるタイル候補（今は「一番後ろ2行の偶数列」）
    TArray<int32> SpawnTileIndices;
    for (int32 Row = Rows - 2; Row < Rows; Row++)
    {
        for (int32 Col = 0; Col < Columns; Col += 2)
        {
            int32 Index = Row * Columns + Col;
            if (EnemyTiles.IsValidIndex(Index))
            {
                SpawnTileIndices.Add(Index);
            }
        }
    }

    if (SpawnTileIndices.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnEnemyUnits: No enemy tiles to spawn on."));
        return;
    }

    int32 TileCursor = 0;

    // ★ Wave 内の「敵グループ」ごとに回す
    for (const FEnemySpawnEntry& Entry : WaveData->Enemies)
    {
        if (!Entry.EnemyClass) continue;

        for (int32 i = 0; i < Entry.Count; ++i)
        {
            if (TileCursor >= SpawnTileIndices.Num())
            {
                UE_LOG(LogTemp, Warning, TEXT("SpawnEnemyUnits: Not enough tiles for all enemies."));
                return;
            }

            int32 TileIndex = SpawnTileIndices[TileCursor++];
            ATile* Tile = EnemyTiles[TileIndex];
            if (!Tile) continue;

            FVector SpawnLocation = Tile->GetActorLocation() + FVector(0, 0, 100.f);

            AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(
                Entry.EnemyClass,
                SpawnLocation,
                FRotator(0, 180, 0)
            );

            if (!NewUnit) continue;

            NewUnit->Team = EUnitTeam::Enemy;
            NewUnit->CurrentTile = Tile;
            NewUnit->OwningBoardManager = this;

            // Waveごとの倍率を反映
            NewUnit->BaseHP *= WaveData->HPScale;
            NewUnit->HP = NewUnit->BaseHP;

            NewUnit->BaseAttack *= WaveData->AttackScale;
            NewUnit->Attack = NewUnit->BaseAttack;

            NewUnit->BaseDefense *= WaveData->DefenseScale;
            NewUnit->Defense = NewUnit->BaseDefense;

            NewUnit->BaseMagicDefense *= WaveData->MagicDefenseScale;
            NewUnit->MagicDefense = NewUnit->BaseMagicDefense;

            NewUnit->BaseMagicPower *= WaveData->MagicPowerScale;
            NewUnit->MagicPower = NewUnit->MagicPower;

            Tile->bIsOccupied = true;
            Tile->OccupiedUnit = NewUnit;

            EnemyUnits.Add(NewUnit);
        }
    }

    UE_LOG(LogTemp, Warning,
        TEXT("SpawnEnemyUnits: Stage=%d Wave=%d, Spawned %d enemies."),
        CurrentStageIndex, CurrentWaveIndex, EnemyUnits.Num());
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
    else if (UnitID == FName("Bear"))
    {
        return PlayerBearClass;
    }
    else if (UnitID == FName("Nurse"))
    {
        return PlayerNurseClass;
    }
    else if (UnitID == FName("Adventurer"))
    {
        return PlayerAdventurerClass;
    }

    // どれにも当てはまらない場合の保険（適当にKnight返すなど）
    return PlayerWizardClass;
}

AUnit* ABoardManager::SpawnRewardUnit(FName UnitID)
{
    TSubclassOf<AUnit>UnitClass = GetPlayerUnitClassByID(UnitID);
    if (!UnitClass)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnRewardUnit: No Class for UnitID %s"), *UnitID.ToString());
        return nullptr;
    }

    if (PlayerManagerInstance)
    {
        int32 CurrentDeployed = GetDeployedPlayerUnitCount();
        if (CurrentDeployed >= PlayerManagerInstance->MaxUnitCount)
        {
            UE_LOG(LogTemp, Warning, TEXT("SpawnRewardUnit: MaxUnitCount reached (%d / %d)."),
                CurrentDeployed, PlayerManagerInstance->MaxUnitCount);
            return nullptr;
        }
    }

    ATile* FreeTile = nullptr;
    for (ATile* Tile : PlayerTiles)
    {
        if (Tile && !Tile->bIsOccupied)
        {
            FreeTile = Tile;
            break;
        }
    }

    if (!FreeTile)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnRewardUnit: No free player tile!"));
        return nullptr;
    }

    FVector SpawnLocation = FreeTile->GetActorLocation() + FVector(0, 0, 100);

    AUnit* NewUnit = GetWorld()->SpawnActor<AUnit>(UnitClass, SpawnLocation, FRotator::ZeroRotator);
    if (!NewUnit)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnRewardUnit: Failed to spawn unit for %s"), *UnitID.ToString());
        return nullptr;
    }

    NewUnit->Team = EUnitTeam::Player;
    NewUnit->OwningBoardManager = this;
    NewUnit->UnitID = UnitID;
    NewUnit->InitialTile = FreeTile;

    MoveUnitToTile(NewUnit, FreeTile);

    PlayerUnits.Add(NewUnit);

    UE_LOG(LogTemp, Warning, TEXT("SpawnRewardUnit: Spawned %s at tile %s"),
        *UnitID.ToString(), *FreeTile->GetName());

    if (PlayerManagerInstance)
    {
        PlayerManagerInstance->RegisterOwnedUnit(UnitID);
    }

    return NewUnit;
}
