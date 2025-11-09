#include "ShopWidget.h"
#include "Components/HorizontalBox.h"
#include "ShopSlotWidget.h"
#include "ItemData.h"
#include "ShopManager.h"

void UShopWidget::UpdateShopUI()
{
    if (!ItemBox || !SlotWidgetClass || !ItemTable || !ShopManager)return;

    ItemBox->ClearChildren();

    for (const FItemData& Item : ShopManager->CurrentItems)
    {
        UShopSlotWidget* SlotWidget = CreateWidget<UShopSlotWidget>(GetOwningPlayer(), SlotWidgetClass);

        if (SlotWidget)
        {
            SlotWidget->ItemName = Item.Name;
            SlotWidget->Price = Item.Price;
            SlotWidget->ShopManagerRef = ShopManager;
            ItemBox->AddChild(SlotWidget);
        }
    }
}


void UShopWidget::UpdateGold(int32 NewGold)
{
    if (GoldText)
    {
        GoldText->SetText(FText::FromString(FString::Printf(TEXT("Gold:%d"), NewGold)));
    }
}

void UShopWidget::RefreshSlots()
{
    for (UWidget* Child : ItemBox->GetAllChildren())
    {
        if (UShopSlotWidget* ItemSlot = Cast<UShopSlotWidget>(Child))
        {
            ItemSlot->UpdateShopState();
        }
    }
}

void UShopWidget::RefreshItemBench()
{
    if (!ItemBench || !ShopManager) return;

    ItemBench->ClearChildren();

    for (const FItemData& Item : ShopManager->HeldItems)
    {
        // 仮：名前だけ表示（あとでスロットに差し替える）
        UTextBlock* Label = NewObject<UTextBlock>(this);
        Label->SetText(Item.Name);

        ItemBench->AddChild(Label);
    }
}
