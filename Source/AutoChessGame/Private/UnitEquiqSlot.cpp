#include "UnitEquiqSlot.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/DragDropOperation.h"   
#include "Unit.h"                          
#include "ItemBenchSlot.h" 
#include "BoardManager.h"
#include "ShopManager.h"

void UUnitEquipSlot::RefreshEquipSlotView()
{
}

bool UUnitEquipSlot::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] NativeOnDrop called"));

    Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

    if (!InOperation)
    {
        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] InOperation is null"));
        return false;
    }

    // ベンチからドラッグされてきたかチェック
    if (UItemBenchSlot* BenchSlot = Cast<UItemBenchSlot>(InOperation->Payload))
    {
        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Cast UItemBenchSlot OK"));

        if (!OwnerUnit)
        {
            UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] OwnerUnit is NULL!!"));
            return false;
        }

        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] OwnerUnit=%s Team=%d"),
            *OwnerUnit->GetName(), (int32)OwnerUnit->Team);

        if (OwnerUnit->Team != EUnitTeam::Player)
        {
            UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Not a player unit, ignore"));
            return false;
        }

        // ★ 1. ShopManager を取得
        AShopManager* ShopManager = nullptr;
        if (OwnerUnit->OwningBoardManager)
        {
            ShopManager = OwnerUnit->OwningBoardManager->ShopManagerRef;
        }

        if (!ShopManager)
        {
            UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] ShopManager is NULL"));
        }

        // ★ 2. 装備スロットの ItemData を更新
        ItemData = BenchSlot->ItemData;

        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Equip Item %s"),
            *ItemData.RowName.ToString());

        // ★ 3. ユニットに装備（ステータス用の内部配列など）
        OwnerUnit->EquipItem(SlotType, ItemData);

        // ★ 4. ベンチから1個削除（RowName で検索）
        if (ShopManager)
        {
            ShopManager->RemoveItemFromBenchByRowName(ItemData.RowName);
        }

        // ★ 5. 見た目更新（スロット側）
        RefreshEquipSlotView();

        // ★ 6. ベンチスロット側も見た目を消したいなら（関数があれば）
        // BenchSlot->ClearSlot();

        // ★ 7. 念のためステータス再適用
        OwnerUnit->ReapplayAllItemEffects();

        return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Cast UItemBenchSlot FAILED"));
    }

    return false;
}