#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardManager.generated.h"

class ATile;
class AUnit;

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

protected:
    virtual void BeginPlay() override;

public:
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
    TSubclassOf<AUnit> PlayerUnitClass;

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
    int32 CurrentRound = 0;

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

    void ProcessBattleTick();

};
