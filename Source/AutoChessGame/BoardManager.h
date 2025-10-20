#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "Unit.h"
#include "BoardManager.generated.h"

UCLASS()
class AUTOCHESSGAME_API ABoardManager : public AActor
{
    GENERATED_BODY()

public:
    ABoardManager();

protected:
    virtual void BeginPlay() override;

public:
    /** �{�[�h�T�C�Y */
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    int32 Rows = 4;
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    int32 Columns = 7;
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    float TileSpacing = 100.f;

    /** �N���X�Q�� */
    UPROPERTY(EditAnywhere, Category = "Board|References")
    TSubclassOf<ATile> TileClass;
    UPROPERTY(EditAnywhere, Category = "Units|References")
    TSubclassOf<AUnit> PlayerUnitClass;
    UPROPERTY(EditAnywhere, Category = "Units|References")
    TSubclassOf<AUnit> EnemyUnitClass;

    /** �^�C���z�� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    TArray<ATile*> PlayerTiles;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    TArray<ATile*> EnemyTiles;

    /** �I�𒆂̃��j�b�g */
    AUnit* SelectedUnit = nullptr;

public:
    void GenerateBoard();
    void SpawnInitialUnits();

    /** �^�C���N���b�N���� */
    void HandleTileClicked(ATile* ClickedTile);
};
