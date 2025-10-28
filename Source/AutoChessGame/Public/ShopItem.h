#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ShopItem.generated.h"

USTRUCT(BlueprintType)
struct FShopItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Price;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;
};
