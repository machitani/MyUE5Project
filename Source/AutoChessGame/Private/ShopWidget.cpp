#include "ShopWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "BoardManager.h"
#include "ItemBenchSlot.h"
#include "Unit.h"
#include "Tile.h"
#include "ShopSlotWidget.h"
#include "ShopManager.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/DragDropOperation.h"
#include "EngineUtils.h"

#include "ItemHoverInfoWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h" 


void UShopWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 既存の初期化があればここに書く（今は無し）

    // ★ ホバー用Widget生成
    if (ItemHoverWidgetClass)
    {
        ItemHoverWidgetInstance = CreateWidget<UItemHoverInfoWidget>(GetWorld(), ItemHoverWidgetClass);
        if (ItemHoverWidgetInstance)
        {
            ItemHoverWidgetInstance->AddToViewport();
            ItemHoverWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void UShopWidget::UpdateShopUI()
{
    if (!ItemBox || !ShopManager || !SlotWidgetClass) return;

    UE_LOG(LogTemp, Warning, TEXT("[ShopWidget] UpdateShopUI: CurrentItems=%d"),
        ShopManager->CurrentItems.Num());

    ItemBox->ClearChildren();
    ShopSlots.Empty();

    for (const FItemData& ItemData : ShopManager->CurrentItems)
    {
        UShopSlotWidget* ShopSlot =
            CreateWidget<UShopSlotWidget>(GetOwningPlayer(), SlotWidgetClass);

        if (!ShopSlot) continue;

        ShopSlot->ItemName = ItemData.Name;
        ShopSlot->Price = ItemData.Price;
        ShopSlot->RowName = ItemData.RowName;
        ShopSlot->ShopManagerRef = ShopManager;
        ShopSlot->OwnerShopWidget = this;

        ShopSlot->RefreshItemView(ItemData); // ここで soldout もリセットされる

        ItemBox->AddChild(ShopSlot);
        ShopSlots.Add(ShopSlot);
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
    ABoardManager* BoardManager = Cast<ABoardManager>(
        UGameplayStatics::GetActorOfClass(this, ABoardManager::StaticClass()));

    if (BoardManager)
    {
        BoardManager->StartBattlePhase();
    }
}

void UShopWidget::ShowItemHover(const FItemData& ItemData)
{
    UE_LOG(LogTemp, Warning, TEXT("[ShopWidget] ShowItemHover: %s"), *ItemData.Name.ToString());

    if (!ItemHoverWidgetInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopWidget] ItemHoverWidgetInstance is NULL"));
        return;
    }

    ItemHoverWidgetInstance->SetItemInfo(ItemData);

    // まずは位置移動は一旦無視して、ただ表示するだけにする
    ItemHoverWidgetInstance->SetVisibility(ESlateVisibility::Visible);
}


void UShopWidget::HideItemHover()
{
    if (ItemHoverWidgetInstance)
    {
        ItemHoverWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }
}
