#include "ItemBenchSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "BoardManager.h"
#include "Unit.h"
#include "ShopManager.h"
#include "ShopWidget.h"
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"

void UItemBenchSlot::NativePreConstruct()
{
    Super::NativePreConstruct();
    RefreshBenchView();
}

void UItemBenchSlot::ClearBenchItem()
{
    ItemData = FItemData();
    bHasItem = false;
    RefreshBenchView();
}

void UItemBenchSlot::SetBenchItem(const FItemData& NewItem)
{
    ItemData = NewItem;
    bHasItem = true;
    RefreshBenchView();
}

void UItemBenchSlot::RefreshBenchView()
{
    if (!ItemIcon || !ItemTextIcon) return;

    if (!bHasItem)
    {
        ItemIcon->SetBrushFromTexture(nullptr);
        ItemTextIcon->SetText(FText::GetEmpty());
        return;
    }

    ItemIcon->SetBrushFromTexture(ItemData.ItemIcon);
    ItemTextIcon->SetText(ItemData.Name);
}

FReply UItemBenchSlot::NativeOnMouseButtonDown(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
    {
        return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    }

    if (!bHasItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BenchSlot] No item to equip"));
        return FReply::Handled();
    }

    // --- 対象ユニット ---
    ABoardManager* BM = Cast<ABoardManager>(
        UGameplayStatics::GetActorOfClass(this, ABoardManager::StaticClass()));
    if (!BM || !BM->ItemUnit)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[BenchSlot] No ItemUnit selected (right-click a unit first)"));
        return FReply::Handled();
    }
    AUnit* TargetUnit = BM->ItemUnit;

    // --- ShopManager取得 ---
    AShopManager* ShopManager = Cast<AShopManager>(
        UGameplayStatics::GetActorOfClass(this, AShopManager::StaticClass()));

    if (ShopManager)
    {
        // ★ 共通関数を呼ぶだけにする
        ShopManager->RemoveItemFromBenchByRowName(ItemData.RowName);
    }

    // --- ユニットに装備 ---
    UE_LOG(LogTemp, Warning,
        TEXT("[BenchSlot] Equip %s to %s"),
        *ItemData.Name.ToString(),
        *TargetUnit->GetName());

    TargetUnit->EquipItem(E_EquiqSlotType::Weapon, ItemData);
    TargetUnit->ReapplayAllItemEffects();

    return FReply::Handled();
}
