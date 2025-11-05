#include "ShopWidget.h"
#include "Components/HorizontalBox.h"
#include "ShopSlotWidget.h"
#include "ItemData.h"
#include "ShopManager.h"

void UShopWidget::UpdateShopUI()
{
    if (!ItemBox || !SlotWidgetClass || !ItemTable || !ShopManager) return;

    ItemBox->ClearChildren();

    TArray<FName> RowNames = ItemTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        const FItemData* Item = ItemTable->FindRow<FItemData>(RowName, TEXT(""));
        if (!Item) continue;

        UShopSlotWidget* SlotWidget = CreateWidget<UShopSlotWidget>(GetOwningPlayer(), SlotWidgetClass);
        if (SlotWidget)
        {
            SlotWidget->ItemName = Item->Name;
            SlotWidget->Price = Item->Price;
            SlotWidget->ShopManagerRef = ShopManager; // ★ これでスロットが購入通知できる
            ItemBox->AddChild(SlotWidget);
        }
    }
}
