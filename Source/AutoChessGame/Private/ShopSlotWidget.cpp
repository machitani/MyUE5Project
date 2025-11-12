#include "ShopSlotWidget.h"
#include "Components/Image.h" 


void UShopSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UShopSlotWidget::UpdateShopState()
{
}

void UShopSlotWidget::RefreshItemView(const FItemData& ItemData)
{
    if (ItemIcon && ItemData.ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(ItemData.ItemIcon);
    }
}


