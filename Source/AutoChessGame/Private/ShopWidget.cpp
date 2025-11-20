#include "ShopWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "BoardManager.h"
#include "ItemBenchSlot.h"
#include "Unit.h"
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
            ShopSlot->RowName = ItemData.RowName;       // ← ここ重要
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
        // ★ ここだけ名前を変える
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
    Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

    UItemBenchSlot* BenchSlot = Cast<UItemBenchSlot>(InOperation->Payload);
    if (!BenchSlot)
    {
        return false;
    }

    const FItemData& ItemData = BenchSlot->ItemData;

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return false;

    const FVector2D DropScreenPos = InDragDropEvent.GetScreenSpacePosition();

    AUnit* BestUnit = nullptr;
    float BestDistSq = FLT_MAX;

    for (TActorIterator<AUnit> It(PC->GetWorld()); It; ++It)
    {
        AUnit* Unit = *It;
        if (!Unit) continue;
        if (Unit->Team != EUnitTeam::Player) continue;
        if (Unit->bIsDead) continue;

        FVector2D UnitScreenPos;
        if (!UGameplayStatics::ProjectWorldToScreen(PC, Unit->GetActorLocation(), UnitScreenPos))
        {
            continue;
        }

        float DistSq = FVector2D::DistSquared(DropScreenPos, UnitScreenPos);

        if (DistSq < BestDistSq)
        {
            BestDistSq = DistSq;
            BestUnit = Unit;
        }
    }

    // ドロップ位置からどれくらい近いかの許容範囲（調整ポイント）
    const float MaxRadiusPx = 400.0f;

    if (BestUnit && BestDistSq <= MaxRadiusPx * MaxRadiusPx)
    {
        BestUnit->EquipItem(E_EquiqSlotType::Weapon, ItemData);
        BestUnit->ReapplayAllItemEffects();

        BenchSlot->ClearBenchItem();

        return true;
    }

    return false;
}
