#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemData.h"
#include "ShopSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UShopWidget;
class UButton;
class AShopManager;   // ★ 追加：前方宣言

UCLASS()
class AUTOCHESSGAME_API UShopSlotWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
    FText ItemName;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
    int32 Price;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
    FName RowName;

    UPROPERTY(meta = (BindWidget))
    UImage* ItemIcon;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
    AShopManager* ShopManagerRef;   // ★ AShopManager にしてOK

    // 親ショップ（ホバー通知先）
    UPROPERTY(BlueprintReadWrite, Category = "Shop")
    UShopWidget* OwnerShopWidget = nullptr;

    // ホバー用に保持するアイテムデータ
    UPROPERTY(BlueprintReadWrite, Category = "Item")
    FItemData CachedItemData;

    // レアリティ色
    UPROPERTY(meta = (BindWidgetOptional))
    UImage* RarityFrame = nullptr;

    // ボタン本体
    UPROPERTY(meta = (BindWidget))
    UButton* Button = nullptr;

    // 売り切れフラグ
    UPROPERTY(BlueprintReadOnly, Category = "Shop")
    bool bIsSoldOut = false;

    // 売り切れカバー（半透明＋テキストなど）
    UPROPERTY(meta = (BindWidgetOptional))
    UWidget* SoldOutCover = nullptr;

    // 売り切れ状態をまとめて反映
    UFUNCTION(BlueprintCallable, Category = "Shop")
    void SetSoldOut(bool bSold);

    UFUNCTION(BlueprintCallable)
    void UpdateShopState();

    UFUNCTION(BlueprintCallable)
    void RefreshItemView(const FItemData& ItemData);

protected:
    UFUNCTION()
    void HandleClicked();
};
