#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "ItemData.h"
#include "ItemBenchSlot.h"
#include "ShopWidget.generated.h"

// 先行宣言（循環参照を避けつつ型名を使えるようにする）
class UShopSlotWidget;
class AShopManager;
class AUnit;
class UItemBechSlot;

UCLASS()
class AUTOCHESSGAME_API UShopWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UHorizontalBox* ItemBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* ItemBench;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    TSubclassOf<UUserWidget> ItemBenchClass;

    // スロットのBP/C++クラス
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

    // ★ここを修正：UWBP_ShopSlot* → UShopSlotWidget*
    // 動的に作る配列なので BindWidgetOptional は外す
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

    //ドロップを受け取る
    virtual bool NativeOnDrop(
        const FGeometry& InGeometry,
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation
    ) override;
};
