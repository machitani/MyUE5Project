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

    UPROPERTY(BlueprintReadWrite)
    float BaseHP;

    UPROPERTY(BlueprintReadWrite)
    float BaseAttack;

    UPROPERTY(BlueprintReadWrite)
    TArray<FItemData> EquippedItems;

    UPROPERTY(BlueprintReadWrite)
    int32 SavedTileIndex;
};
