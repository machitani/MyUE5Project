#include "ShopWidget.h"
#include "Components/HorizontalBox.h"
#include "ShopSlotWidget.h"
#include "ItemData.h"
#include "ShopManager.h"

void UShopWidget::UpdateShopUI()
{
    UE_LOG(LogTemp, Warning, TEXT("UpdateShopUI Called"));

    if (!ItemBox)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemBox is NULL!!"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("ItemBox OK"));

    UE_LOG(LogTemp, Warning, TEXT("SlotWidgetClass = %s"), *GetNameSafe(SlotWidgetClass));
    UE_LOG(LogTemp, Warning, TEXT("ItemTable = %s"), *GetNameSafe(ItemTable));
    UE_LOG(LogTemp, Warning, TEXT("ShopManager = %s"), *GetNameSafe(ShopManager));

    if (!SlotWidgetClass || !ItemTable || !ShopManager)
    {
        UE_LOG(LogTemp, Error, TEXT("One of the required references is NULL!"));
        return;
    }

    ItemBox->ClearChildren();

    TArray<FName> RowNames = ItemTable->GetRowNames();
    UE_LOG(LogTemp, Warning, TEXT("Row Count = %d"), RowNames.Num());

    for (const FName& RowName : RowNames)
    {
        const FItemData* Item = ItemTable->FindRow<FItemData>(RowName, TEXT("UpdateShopUI"));

        if (!Item)
        {
            UE_LOG(LogTemp, Error, TEXT("FindRow failed for %s"), *RowName.ToString());
            continue;
        }

        UShopSlotWidget* SlotWidget = CreateWidget<UShopSlotWidget>(GetOwningPlayer(), SlotWidgetClass);
        if (SlotWidget)
        {
            SlotWidget->ItemName = Item->Name;
            SlotWidget->Price = Item->Price;
            SlotWidget->ShopManagerRef = ShopManager;
            ItemBox->AddChild(SlotWidget);
            UE_LOG(LogTemp, Warning, TEXT("Added Slot: %s (%d G)"), *Item->Name.ToString(), Item->Price);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("CreateWidget FAILED!"));
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

