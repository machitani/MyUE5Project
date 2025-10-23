#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardManager.generated.h"

class ATile;
class AUnit;

UCLASS()
class AUTOCHESSGAME_API ABoardManager : public AActor
{
    GENERATED_BODY()

public:
    ABoardManager();

protected:
    virtual void BeginPlay() override;

public:
    /** �{�[�h�ݒ� */
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    int32 Rows = 4;

    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    int32 Columns = 7;

    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    float TileSpacing = 100.f;

    /** �^�C���N���X�iBP �ō����ւ��邱�ƑO��j */
    UPROPERTY(EditAnywhere, Category = "Board|References")
    TSubclassOf<class ATile> TileClass;

    /** ���j�b�g�N���X */
    UPROPERTY(EditAnywhere, Category = "Units|References")
    TSubclassOf<AUnit> PlayerUnitClass;

    UPROPERTY(EditAnywhere, Category = "Units|References")
    TSubclassOf<AUnit> EnemyUnitClass;

    /** �����Ǘ� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    TArray<ATile*> PlayerTiles;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    TArray<ATile*> EnemyTiles;

    /** �I�𒆂̃��j�b�g */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    AUnit* SelectedUnit;

public:
    /** �{�[�h���� */
    void GenerateBoard();

    /** �������j�b�g���� */
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

};
