#include "ShopWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "BoardManager.h"
#include "ItemBenchSlot.h"
#include "ShopSlotWidget.h"
#include "ShopManager.h"

void UShopWidget::UpdateShopUI()
{
    if (!ItemBox || !ShopManager || !SlotWidgetClass|| !ItemTable) return;

    ItemBox->ClearChildren();
    //TArray<FName> RowNames = ShopManager->ItemTable->GetRowNames();

    for (const FItemData& ItemData:ShopManager->CurrentItems)
    {
        UShopSlotWidget* ShopSlot = CreateWidget<UShopSlotWidget>(GetOwningPlayer(), SlotWidgetClass);
     
        if (ShopSlot)
        {
            ShopSlot->ItemName = ItemData.Name;
            ShopSlot->Price = ItemData.Price;
            ShopSlot->RowName = ItemData.RowName;       // © ‚±‚±d—v
            ShopSlot->ShopManagerRef = ShopManager;

            ShopSlot->RefreshItemView(ItemData);

            ItemBox->AddChild(ShopSlot);
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
    if (!ItemBench || !ItemBenchClass || !ShopManager) return;

    ItemBench->ClearChildren();

    for (const FItemData& Item : ShopManager->BenchItems)
    {
        UItemBenchSlot* BenchSlot = CreateWidget<UItemBenchSlot>(GetOwningPlayer(), ItemBenchClass);
        if (BenchSlot)
        {
            BenchSlot->ItemData = Item;
            ItemBench->AddChildToVerticalBox(BenchSlot);
        }
    }
}

void UShopWidget::OnBuyExpButtonPressed()
{
    if (ShopManager)
    {
        ShopManager->BuyExp();
    }
}

void UShopWidget::OnReadyButtonClicked()
{
    ABoardManager* BoardManager = Cast<ABoardManager>(UGameplayStatics::GetActorOfClass(this, ABoardManager::StaticClass()));

    if (BoardManager)
    {
        BoardManager->StartBattlePhase();
    }
}
