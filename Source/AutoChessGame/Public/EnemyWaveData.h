#pragma once

#include "CoreMinimal.h"
#include "EnemyWaveData.generated.h"

class AUnit;

// ★ 1種類の敵の「クラス＋何体出すか」
USTRUCT(BlueprintType)
struct FEnemySpawnEntry
{
    GENERATED_BODY();

    // 出したい敵クラス
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    TSubclassOf<AUnit> EnemyClass;

    // その敵を何体出すか
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 Count = 1;
};

// ★ Wave 全体の情報
USTRUCT(BlueprintType)
struct FEnemyWaveData
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 StageIndex = 1;

    // 何 Wave 目か（1,2,3…）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 WaveIndex = 1;

    // ★ この Wave で出す敵たち（複数種類OK）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    TArray<FEnemySpawnEntry> Enemies;

    // 全体にかける倍率
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float HPScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float AttackScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float DefenseScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float MagicDefenseScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float MagicPowerScale = 1.0f;
};
