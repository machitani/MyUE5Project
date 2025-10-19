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

    /** ===== ボード設定 ===== */

    // 行数（縦）
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    int32 Rows = 4;

    // 列数（横）
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    int32 Columns = 7;

    // タイル間隔
    UPROPERTY(EditAnywhere, Category = "Board|Settings")
    float TileSpacing = 100.f;

    // タイルのクラス
    UPROPERTY(EditAnywhere, Category = "Board|References")
    TSubclassOf<ATile> TileClass;

    /** ===== ユニット設定 ===== */

    UPROPERTY(EditAnywhere, Category = "Units|References")
    TSubclassOf<AUnit> PlayerUnitClass;

    UPROPERTY(EditAnywhere, Category = "Units|References")
    TSubclassOf<AUnit> EnemyUnitClass;


    /** ===== 内部管理 ===== */

    // プレイヤー側タイル
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    TArray<ATile*> PlayerTiles;

    // 敵側タイル
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board|Runtime")
    TArray<ATile*> EnemyTiles;

    

public:
    void GenerateBoard();
    void SpawnInitialUnits();
};
