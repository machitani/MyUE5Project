#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "ItemData.h"
#include "ItemBenchSlot.h"
#include "ShopWidget.generated.h"

class UShopSlotWidget;
class AShopManager;
class AUnit;
class UItemBechSlot;
class UItemHoverInfoWidget;

UCLASS()
class AUTOCHESSGAME_API UShopWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;   // ★ 追加

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UHorizontalBox* ItemBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* ItemBench;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    TSubclassOf<UUserWidget> ItemBenchClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    TSubclassOf<UShopSlotWidget> SlotWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    TArray<FItemData> Items;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    UDataTable* ItemTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    AShopManager* ShopManager;

    UPROPERTY(BlueprintReadWrite, Category = "Shop")
    TArray<FItemData> BenchItems;

    UPROPERTY(BlueprintReadWrite, Category = "Shop")
    TArray<UShopSlotWidget*> ShopSlots;

    UFUNCTION(BlueprintCallable) void UpdateShopUI();
    UPROPERTY(meta = (BindWidget)) UTextBlock* GoldText;
    UFUNCTION(BlueprintCallable) void UpdateGold(int32 NewGold);
    UFUNCTION(BlueprintCallable) void RefreshSlots();
    UFUNCTION(BlueprintCallable) void RefreshItemBench();

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* ItemBenchBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bench")
    TSubclassOf<UItemBenchSlot> ItemBenchSlotClass;

    UPROPERTY()
    TArray<UItemBenchSlot*> ItemBenchSlots;

    UFUNCTION(BlueprintCallable)
    void OnBuyExpButtonPressed();

    UFUNCTION(BlueprintCallable)
    void OnReadyButtonClicked();

    UFUNCTION(BlueprintCallable, Category = "Hover")
    void ShowItemHover(const FItemData& ItemData);

    UFUNCTION(BlueprintCallable, Category = "Hover")
    void HideItemHover();

    // ★ 追加：ホバー情報用のWidgetクラスとインスタンス
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
    TSubclassOf<UItemHoverInfoWidget> ItemHoverWidgetClass;

    UPROPERTY()
    UItemHoverInfoWidget* ItemHoverWidgetInstance = nullptr;
};
