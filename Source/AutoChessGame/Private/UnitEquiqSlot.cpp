#include "UnitEquiqSlot.h"

#include "Blueprint/DragDropOperation.h"
#include "Unit.h"
#include "ItemBenchSlot.h"
#include "BoardManager.h"
#include "ShopManager.h"

void UUnitEquipSlot::RefreshEquipSlotView()
{
    // TODO: スロットアイコン等を ItemData から更新する処理
}

bool UUnitEquipSlot::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] NativeOnDrop called"));

    if (!InOperation)
    {
        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] InOperation is null"));
        return false;
    }

    // Payload がベンチスロットかチェック
    UItemBenchSlot* BenchSlot = Cast<UItemBenchSlot>(InOperation->Payload);
    if (!BenchSlot)
    {
        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Payload is not UItemBenchSlot"));
        return false;
    }

    // OwnerUnit チェック
    if (!OwnerUnit)
    {
        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] OwnerUnit is NULL"));
        return false;
    }

    // プレイヤー以外は装備不可
    if (OwnerUnit->Team != EUnitTeam::Player)
    {
        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Not a player unit, ignore"));
        return false;
    }

    // ベンチ側のアイテム取得（※ここでは EquipSlot の ItemData をまだ変えない）
    const FItemData DragItem = BenchSlot->ItemData;

    // 空データっぽい時の保険（RowName が None など）
    if (DragItem.RowName.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] DragItem RowName is None (empty slot?)"));
        return false;
    }

    UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Try equip item: %s"),
        *DragItem.RowName.ToString());

    // ★ 装備できるか判定＆実装（ここで失敗したらベンチは絶対減らさない）
    const bool bEquipped = OwnerUnit->TryEquipItem(SlotType, DragItem);
    if (!bEquipped)
    {
        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Equip failed (limit or rule). Keep item on bench."));
        return false; // ドロップ失敗扱い → UI的に元の場所に戻る
    }

    // ---- ここから下は「成功した時だけ」実行する ----

    // ShopManager 取得（成功時のみ）
    AShopManager* ShopManager = nullptr;
    if (OwnerUnit->OwningBoardManager)
    {
        ShopManager = OwnerUnit->OwningBoardManager->ShopManagerRef;
    }

    if (!ShopManager)
    {
        UE_LOG(LogTemp, Error, TEXT("[EquipSlot] ShopManager is NULL. Equipped, but cannot remove from bench!"));

        // ここで return true/false は好みだけど、
        // 今回は「ベンチを減らせないなら問題が見えやすいように false にして戻す」
        // ※ただしすでにTryEquipItemでステータスは反映済みなので、
        //   もしこのケースが起きるなら設計的に ShopManagerRef の設定を直すのが本筋。
        return false;
    }

    // ★ 成功した時だけベンチから削除
    ShopManager->RemoveItemFromBenchByRowName(DragItem.RowName);

    // ★ 成功した時だけ EquipSlot の ItemData を更新
    ItemData = DragItem;

    // 見た目更新
    RefreshEquipSlotView();

    UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Equip success: %s"),
        *ItemData.RowName.ToString());

    return true;
}
