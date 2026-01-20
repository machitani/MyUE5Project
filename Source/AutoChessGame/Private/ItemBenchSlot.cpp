#include "ItemBenchSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "BoardManager.h"
#include "Unit.h"
#include "ShopManager.h"
#include "ShopWidget.h"
#include "CoreMinimal.h"
#include "ItemHoverInfoWidget.h"
#include "Kismet/GameplayStatics.h"

void UItemBenchSlot::NativePreConstruct()
{
    Super::NativePreConstruct();
    RefreshBenchView();
}

void UItemBenchSlot::NativeConstruct()
{
    Super::NativeConstruct();

    // ★ Button に OnClicked をバインド
    if (ClickArea)
    {
        ClickArea->OnClicked.AddDynamic(this, &UItemBenchSlot::HandleClicked);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[BenchSlot] ClickArea is NULL (BindWidget missing?)"));
    }
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

void UItemBenchSlot::HandleClicked()
{
    UE_LOG(LogTemp, Warning,
        TEXT("[BenchSlot] HandleClicked: bHasItem=%d Name=%s"),
        bHasItem ? 1 : 0,
        *ItemData.Name.ToString());

    if (!bHasItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BenchSlot] No item in this slot."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[BenchSlot] World is NULL"));
        return;
    }

    // --- BoardManager 取得 ---
    ABoardManager* BM = Cast<ABoardManager>(
        UGameplayStatics::GetActorOfClass(World, ABoardManager::StaticClass()));
    if (!BM)
    {
        UE_LOG(LogTemp, Error, TEXT("[BenchSlot] BoardManager not found"));
        return;
    }

    if (!BM->ItemUnit)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[BenchSlot] No ItemUnit selected (click a unit first)"));
        return;
    }

    AUnit* TargetUnit = BM->ItemUnit;

    // --- ShopManager 取得（削除に必要） ---
    AShopManager* ShopManager = Cast<AShopManager>(
        UGameplayStatics::GetActorOfClass(World, AShopManager::StaticClass()));
    if (!ShopManager)
    {
        UE_LOG(LogTemp, Error, TEXT("[BenchSlot] ShopManager not found"));
        return;
    }

    // ★ 先に「装備できるか」判定（成功したら装備＆ベンチ削除）
    const bool bEquipped = TargetUnit->TryEquipItem(E_EquiqSlotType::Weapon, ItemData);
    if (!bEquipped)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BenchSlot] Equip failed (limit), keep item on bench"));
        return; // ★ ここで終了：ベンチは減らさない
    }

    // ★ 成功した時だけベンチから削除
    const bool bRemoved = ShopManager->RemoveItemFromBenchByRowName(ItemData.RowName);
    UE_LOG(LogTemp, Warning,
        TEXT("[BenchSlot] RemoveItemFromBenchByRowName(%s) -> %s"),
        *ItemData.RowName.ToString(),
        bRemoved ? TEXT("true") : TEXT("false"));

    TargetUnit->RefreshHoverInfo();

    // ※ ReapplayAllItemEffects は基本いらない（TryEquipItem内で反映してる）
    // TargetUnit->ReapplayAllItemEffects();
}

