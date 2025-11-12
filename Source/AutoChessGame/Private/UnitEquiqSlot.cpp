#include "UnitEquiqSlot.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/DragDropOperation.h"   
#include "Unit.h"                          
#include "ItemBenchSlot.h"                

void UUnitEquipSlot::RefreshEquipSlotView()
{
}

bool UUnitEquipSlot::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

    if (UItemBenchSlot* BenchSlot = Cast<UItemBenchSlot>(InOperation->Payload))
    {
        ItemData = BenchSlot->ItemData; // ドロップされたアイテムデータ受け取り

        if (OwnerUnit)
        {
            OwnerUnit->EquipItem(SlotType, ItemData); //装備処理呼び出し
        }

        // 見た目を更新（アイコン反映など）
        RefreshEquipSlotView();

        return true;
    }

    return false;
}
