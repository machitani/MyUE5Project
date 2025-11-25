#include "ShopWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "BoardManager.h"
#include "ItemBenchSlot.h"
#include "Unit.h"
#include "Tile.h"
#include "BoardManager.h"
#include "ShopSlotWidget.h"
#include "ShopManager.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/DragDropOperation.h"
#include "EngineUtils.h"

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
    if (!ShopManager || !ItemBenchBox || !ItemBenchSlotClass) return;

    ItemBenchBox->ClearChildren();
    ItemBenchSlots.Empty();

    for (const FItemData& Item : ShopManager->BenchItems)
    {
        // š ‚±‚±‚¾‚¯–¼‘O‚ğ•Ï‚¦‚é
        UItemBenchSlot* NewBenchSlot = CreateWidget<UItemBenchSlot>(GetWorld(), ItemBenchSlotClass);
        if (!NewBenchSlot) continue;

        NewBenchSlot->SetBenchItem(Item);

        ItemBenchBox->AddChild(NewBenchSlot);
        ItemBenchSlots.Add(NewBenchSlot);
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

