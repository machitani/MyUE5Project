// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyWaveData.generated.h"

class AUnit;

// EnemyWave 用のデータ
USTRUCT(BlueprintType)
struct FEnemyWaveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Wave")
    int32 StageIndex = 1;

    // 何 Wave 目か（1,2,3…）※なくても動くけど分かりやすさ用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 WaveIndex = 1;

    // この Wave で出す敵の数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 EnemyCount = 2;

    // この Wave で使う敵クラス（今の EnemyUnitClass と同じ系統）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    TSubclassOf<AUnit> EnemyClass;

    // HP の倍率（1.0 = そのまま、1.5 = 1.5倍）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float HPScale = 1.0f;

    // 攻撃力の倍率（必要になったら使う）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float AttackScale = 1.0f;
};
