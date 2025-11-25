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
    // 左クリックだけ扱う
    if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
    {
        return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    }

    if (!bHasItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BenchSlot] No item to equip"));
        return FReply::Handled();
    }

    // --- BoardManager から「今アイテムをつけるユニット」を取得 ---
    ABoardManager* BM = Cast<ABoardManager>(
        UGameplayStatics::GetActorOfClass(this, ABoardManager::StaticClass()));
    if (!BM || !BM->ItemUnit)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[BenchSlot] No ItemUnit selected (right-click a unit first)"));
        return FReply::Handled();
    }

    AUnit* TargetUnit = BM->ItemUnit;

    // --- ShopManager を取得 ---
    AShopManager* ShopManager = Cast<AShopManager>(
        UGameplayStatics::GetActorOfClass(this, AShopManager::StaticClass()));

    if (ShopManager)
    {
        // ★ BenchItems から「このスロットの ItemData に対応するやつ」を1個だけ探す
        int32 IndexToRemove = INDEX_NONE;

        for (int32 i = 0; i < ShopManager->BenchItems.Num(); ++i)
        {
            const FItemData& Data = ShopManager->BenchItems[i];

            // 「同じアイテム」とみなす条件は RowName で判定（必要なら他も見る）
            if (Data.RowName == ItemData.RowName)
            {
                IndexToRemove = i;
                break;
            }
        }

        if (IndexToRemove != INDEX_NONE)
        {
            ShopManager->BenchItems.RemoveAt(IndexToRemove);
            UE_LOG(LogTemp, Warning,
                TEXT("[BenchSlot] Removed bench item at index %d"), IndexToRemove);
        }
        else
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[BenchSlot] No matching item in BenchItems"));
        }

        // ベンチUIを作り直す
        if (ShopManager->ShopWidget)
        {
            ShopManager->ShopWidget->RefreshItemBench();
        }
    }


    // --- ユニットに装備 ---
    UE_LOG(LogTemp, Warning,
        TEXT("[BenchSlot] Equip %s to %s"),
        *ItemData.Name.ToString(),
        *TargetUnit->GetName());

    TargetUnit->EquipItem(E_EquiqSlotType::Weapon, ItemData);
    TargetUnit->ReapplayAllItemEffects();

    // このスロット自体は RefreshItemBench で作り直されるから、
    // 見た目をいじる必要はない
    // ClearBenchItem();

    return FReply::Handled();
}