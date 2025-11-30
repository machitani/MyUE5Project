#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemData.h"
#include "ShopSlotWidget.generated.h"

class UImage;
class UShopWidget;

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
    class AShopManager* ShopManagerRef;

    // 親ショップ（ホバー通知先）
    UPROPERTY(BlueprintReadWrite, Category = "Shop")
    UShopWidget* OwnerShopWidget = nullptr;

    // ★ ホバー用に保持するアイテムデータ
    UPROPERTY(BlueprintReadWrite, Category = "Item")
    FItemData CachedItemData;

    UFUNCTION(BlueprintCallable)
    void UpdateShopState();

    UFUNCTION(BlueprintCallable)
    void RefreshItemView(const FItemData& ItemData);
};
