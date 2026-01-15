#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyWaveData.h"
#include "BoardManager.generated.h"

class ATile;
class AUnit;
class APlayerManager;
class AShopManager;
class UUserWidget;
class UPlayerHUD;

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    Preparation UMETA(DisplayName = "Preparation"),
    Battle UMETA(DisplayName = "Battle"),
    Result UMETA(DisplayName = "Result")
};


UCLASS()
class AUTOCHESSGAME_API ABoardManager : public AActor
{
    GENERATED_BODY()

public:
    ABoardManager();

    /** ボード設定 */
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    int32 Rows = 4;

    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    int32 Columns = 7;

    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    float TileSpacing = 100.f;

    /** タイルクラス（BP で差し替えること前提） */
    UPROPERTY(EditAnywhere, Category = "Board|References")
    TSubclassOf<class ATile> TileClass;

    /** ユニットクラス */
    UPROPERTY(EditAnywhere, Category = "Units|References")
    TSubclassOf<AUnit> PlayerKnightClass;

    UPROPERTY(EditAnywhere,Category="Units|References")
    TSubclassOf<AUnit>PlayerWizardClass;

    UPROPERTY(EditAnywhere,Category="Units|References")
    TSubclassOf<AUnit> PlayerArcherClass;

    UPROPERTY(EditAnywhere,Category="Unit|References")
    TSubclassOf<AUnit> PlayerBearClass;

    UPROPERTY(EditAnywhere,Category="Unit|References")
    TSubclassOf<AUnit> PlayerNurseClass;

    UPROPERTY(EditAnywhere,Category="Unit|References")
    TSubclassOf<AUnit>PlayerAdventurerClass;

    UPROPERTY(EditAnywhere, Category = "Unit|References")
    TSubclassOf<AUnit>PlayerRabbitClass;

    UPROPERTY(EditAnywhere, Category = "Units|References")
    TSubclassOf<AUnit> EnemyUnitClass;

    /** 内部管理 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    TArray<ATile*> PlayerTiles;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    TArray<ATile*> EnemyTiles;

    /** 選択中のユニット */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    AUnit* SelectedUnit;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item")
    AUnit* ItemUnit = nullptr;

    UFUNCTION()
    void SetItemTargetUnit(AUnit* NewUnit);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AShopManager* ShopManagerRef;

    // 1D配列 index 変換
    UFUNCTION(BlueprintPure, Category = "Board|Tiles")
    int32 ToIndex(int32 Row, int32 Col) const;

    // タイル取得
    UFUNCTION(BlueprintPure, Category = "Board|Tiles")
    ATile* GetEnemyTile(int32 Row, int32 Col) const;

    UFUNCTION(BlueprintPure, Category = "Board|Tiles")
    ATile* GetPlayerTile(int32 Row, int32 Col) const;

    // 敵ボス用フォーメーション（遠距離Row0 / ボスRow1 / タンクRow3）
    UFUNCTION(BlueprintCallable, Category = "Enemy|Formation")
    void ArrangeEnemyBossFormation();


public:
    /** ボード生成 */
    void GenerateBoard();

    /** 初期ユニット生成 */
    void SpawnInitialUnits();

    UFUNCTION(BlueprintCallable, Category = "Tile")
    void HandleTileClicked(ATile* ClickedTile);

    //TArray<ATile*> PlayerTiles;
    TArray<AUnit*> PlayerUnits;
    TArray<AUnit*>EnemyUnits;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
    int32 CurrentRound = 1;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
    float TurnInterval = 1.0f;

    FTimerHandle RoundTimerHandle;

    UFUNCTION()
    void StartNextRound();

    UFUNCTION()
    void ProcessEnemyTurn();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Phase")
    EGamePhase CurrentPhase = EGamePhase::Preparation;

    UFUNCTION(BlueprintCallable, Category = "Game Phase")
    void StartBattlePhase();

    UFUNCTION(BlueprintCallable, Category = "Game Phase")
    void EndBattlePhase();

    UFUNCTION(BlueprintCallable, Category = "Round")
    void StartPreparationPhase();

    UFUNCTION(BlueprintCallable,Category="Round")
    void StartResultPhase();

    void ResetBoardForNextRound();

    void ProcessBattleTick();

    // BoardManager.h の public: あたりに追加
    void RebuildPlayerTileOccupancy();


    //FTimerHandle RoundTimerHandle;    // BattlePhase 用
    FTimerHandle PhaseTimerHandle;    // Preparation / Result 用

    // HUD
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> HUDClass;

    UPROPERTY()
    UUserWidget* HUDInstance;

    // HUD 更新関数
    void UpdateHUD();

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Manager")
    TSubclassOf<class APlayerManager> PlayerManagerClass;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Manager")
    APlayerManager* PlayerManagerInstance;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="UI")
    TSubclassOf<UUserWidget> ShopWidgetClass;

    UUserWidget* ShopInstance = nullptr;

    UFUNCTION(BlueprintCallable)
    void OpenShop();

    UPROPERTY(EditAnywhere,Category="UI")
    TSubclassOf<UUserWidget>PlayerHUDClass;

    UPlayerHUD* PlayerHUDInstance;

    UPROPERTY()
    bool bRoundEnded = false;

    void MoveUnitToTile(class AUnit* Unit, class ATile* NewTile);
    ATile* GetTileUnderLocation(const FVector& Location);

    void SpawnPlayerUnitsFromSaveData();

    void SpawnEnemyUnits();

    UFUNCTION(BlueprintCallable,Category="Board|Unit")
    int32 GetDeployedPlayerUnitCount() const;
    
    UFUNCTION()
    TSubclassOf<AUnit> GetPlayerUnitClassByID(FName UnitID) const;

    UFUNCTION()
    AUnit* SpawnRewardUnit(FName UnitID);

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Reward")
    TArray<FName>RewardUnitIDList;

    TArray<FName> GenerateRewardUnitCandidates(int32 Num)const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyWave")
    TArray<FEnemyWaveData> EnemyWaves;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="EnemyWave")
    int32 CurrentStageIndex = 1;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="EnemyWave")
    int32 CurrentWaveIndex = 1;

    FEnemyWaveData* GetCurrentWaveData();

    // ゲーム終了フラグ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
    bool bIsGameOver = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
    bool bIsGameClear = false;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
    bool bLastRoundWasVictory = false;

    UFUNCTION()
    void ReviveAllEnemiesOnDefeat();


    FLinearColor GetStageTileColor()const;

    void ApplyTileColors();

    //ハンドラ関数
    void HandleGameOver();
    void HandleGameClear();
    void HandleDefeat();

    UPROPERTY(EditAnywhere, Category = "Stage")
    UDataTable* Stage1WaveTable;

    UPROPERTY(EditAnywhere, Category = "Stage")
    UDataTable* Stage2WaveTable;

    UPROPERTY(EditAnywhere, Category = "Stage")
    UDataTable* Stage3WaveTable;

    void StartStage(int32 Stage);

    void LoadEnemyWavesFromTable(UDataTable* Table);

    protected:
        virtual void BeginPlay() override;

      private:
          bool bBattleRequested = false;
};
