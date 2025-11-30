#include "ShopSlotWidget.h"
#include "ShopWidget.h"
#include "ItemData.h"
#include "Components/Image.h"

void UShopSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UShopSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    UE_LOG(LogTemp, Warning, TEXT("[ShopSlot] MouseEnter %s"), *RowName.ToString());

    if (OwnerShopWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopSlot] Call ShowItemHover"));
        OwnerShopWidget->ShowItemHover(CachedItemData);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopSlot] OwnerShopWidget is NULL"));
    }
}


void UShopSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (OwnerShopWidget)
    {
        // 詳細非表示
        OwnerShopWidget->HideItemHover();
    }
}

void UShopSlotWidget::UpdateShopState()
{
    // 必要になったら書く。今は空でOK
}

void UShopSlotWidget::RefreshItemView(const FItemData& ItemData)
{
    // ★ ホバー用にフルデータを保持
    CachedItemData = ItemData;

    if (ItemIcon && ItemData.ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(ItemData.ItemIcon);
    }
    else if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(nullptr);
    }
}
