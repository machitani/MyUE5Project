#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "ItemData.h"
#include "ShopSlotWidget.h"
#include "ShopWidget.generated.h"

UCLASS()
class AUTOCHESSGAME_API UShopWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    //virtual void NativeConstruct() override;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UHorizontalBox* ItemBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    TSubclassOf<UShopSlotWidget> SlotWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    TArray<FItemData> Items;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    UDataTable* ItemTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    class AShopManager* ShopManager;

    UFUNCTION(BlueprintCallable)
    void UpdateShopUI();

    UPROPERTY(meta=(BindWidget))
    class UTextBlock* GoldText;

    UFUNCTION(BlueprintCallable)
    void UpdateGold(int32 NewGold);

    UFUNCTION(BlueprintCallable)
    void RefreshSlots();

    UFUNCTION(BlueprintCallable)
    void RefreshItemBench();

};
