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
            ShopSlot->RowName = ItemData.RowName;       // Å© Ç±Ç±èdóv
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
        // Åö Ç±Ç±ÇæÇØñºëOÇïœÇ¶ÇÈ
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

bool UShopWidget::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("[ShopWidget] NativeOnDrop called"));

    Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

    UItemBenchSlot* BenchSlot = Cast<UItemBenchSlot>(InOperation ? InOperation->Payload : nullptr);
    if (!BenchSlot)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopWidget] BenchSlot cast FAILED"));
        return false;
    }

    const FItemData& ItemData = BenchSlot->ItemData;

    ABoardManager* BoardManager = Cast<ABoardManager>(
        UGameplayStatics::GetActorOfClass(this, ABoardManager::StaticClass()));

    if (!BoardManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopWidget] BoardManager not found"));
        return false;
    }

    AUnit* TargetUnit = BoardManager->ItemUnit;
    if (!TargetUnit)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[ShopWidget] ItemUnit is NULL (no unit selected for item)"));
        return false;
    }

    UE_LOG(LogTemp, Warning, TEXT("[ShopWidget] Equip item to %s"),
        *TargetUnit->GetName());

    TargetUnit->EquipItem(E_EquiqSlotType::Weapon, ItemData);
    TargetUnit->ReapplayAllItemEffects();
    BenchSlot->ClearBenchItem();

    return true;
}
