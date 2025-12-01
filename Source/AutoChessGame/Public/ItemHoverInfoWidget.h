#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemData.h"
#include "ItemHoverInfoWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class AUTOCHESSGAME_API UItemHoverInfoWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Item")
    void SetItemInfo(const FItemData& ItemData);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemNameText = nullptr;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PriceText = nullptr;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EffectText = nullptr;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RarityText = nullptr;

   
};
