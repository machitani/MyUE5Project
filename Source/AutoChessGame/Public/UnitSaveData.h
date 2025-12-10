#pragma once
#include "CoreMinimal.h"
#include "ItemData.h"
#include "UnitSaveData.generated.h"

USTRUCT(BlueprintType)
struct FUnitSaveData
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite)
    FName UnitID;

    // ---- 基礎ステータス（アイテム適用前の素の値） ----
    UPROPERTY(BlueprintReadWrite)
    float BaseHP = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float BaseAttack = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float BaseDefense = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float BaseMagicPower = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float BaseMagicDefense = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float BaseRange = 0.f;

    UPROPERTY(BlueprintReadWrite)
    float BaseMoveSpeed = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CritChance = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CritMultiplier = 1.5f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float BaseAttackInterval = 1.0f;

    // ---- 装備アイテム ----
    UPROPERTY(BlueprintReadWrite)
    TArray<FItemData> EquippedItems;

    // ---- どのタイルにいたか ----
    UPROPERTY(BlueprintReadWrite)
    int32 SavedTileIndex = -1;
};
