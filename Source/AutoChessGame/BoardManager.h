// Fill out your copyright notice in the Description page of Project Settings.

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

    /** ===== �{�[�h�ݒ� ===== */

    // �s���i�c�j
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    int32 Rows = 4;

    // �񐔁i���j
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    int32 Columns = 7;

    // �^�C���Ԋu
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    float TileSpacing = 100.f;

    // �^�C���̃N���X
    UPROPERTY(EditAnywhere, Category = "Board|References")
    TSubclassOf<ATile> TileClass;

    /** ===== ���j�b�g�ݒ� ===== */

    UPROPERTY(EditAnywhere, Category = "Units|References")
    TSubclassOf<AUnit> PlayerUnitClass;

    UPROPERTY(EditAnywhere, Category = "Units|References")
    TSubclassOf<AUnit> EnemyUnitClass;


    /** ===== �����Ǘ� ===== */

    // �v���C���[���^�C��
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    TArray<ATile*> PlayerTiles;

    // �G���^�C��
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    TArray<ATile*> EnemyTiles;

    

public:
    void GenerateBoard();
    void SpawnInitialUnits();
};
