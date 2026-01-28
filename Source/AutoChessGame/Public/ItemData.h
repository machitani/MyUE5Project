#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    Common UMETA(DisplayName="Common"),
    Rare   UMETA(DisplayName="Rare"),
    Epic   UMETA(DisplayName="Epic")
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name = FText::GetEmpty();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Price = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RowName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* ItemIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString EffectType = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EffectValue = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemRarity Rarity = EItemRarity::Common;
};