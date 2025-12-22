#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"      // ★これが必要
#include "EnemyWaveData.generated.h"

class AUnit;

USTRUCT(BlueprintType)
struct FEnemySpawnEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AUnit> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FEnemyWaveData : public FTableRowBase   // ★これが重要
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WaveIndex = 1;

    // ステージ別DTにするなら、StageIndexは無くてもOK
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StageIndex = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FEnemySpawnEntry> Enemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HPScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DefenseScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MagicDefenseScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MagicPowerScale = 1.0f;
};
