#include "ShopWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "ItemBenchSlot.h"
#include "ShopSlotWidget.h"
#include "ShopManager.h"

void UShopWidget::UpdateShopUI()
{
    if (!ItemBox || !ShopManager || !ShopManager->ItemTable) return;

    ItemBox->ClearChildren();
    TArray<FName> RowNames = ShopManager->ItemTable->GetRowNames();

    for (int32 i = 0; i < ShopManager->CurrentItems.Num(); i++)
    {
        UShopSlotWidget* ShopSlot = CreateWidget<UShopSlotWidget>(GetOwningPlayer(), SlotWidgetClass);
        ItemBox->AddChild(ShopSlot);
        ShopSlot->ItemName = ShopManager->CurrentItems[i].Name;
        ShopSlot->Price = ShopManager->CurrentItems[i].Price;
        ShopSlot->ShopManagerRef = ShopManager;

        ItemBox->AddChild(ShopSlot);
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
    if (!ItemBench || !ItemBenchClass || !ShopManager) return;

    ItemBench->ClearChildren();

    for (const FItemData& Item : ShopManager->BenchItems)
    {
        UItemBenchSlot* BenchSlot = CreateWidget<UItemBenchSlot>(GetOwningPlayer(), ItemBenchClass);
        ItemBench->AddChild(BenchSlot);
        BenchSlot->ItemData = Item;

    }
}