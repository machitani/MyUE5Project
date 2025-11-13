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
        ItemData = BenchSlot->ItemData;

        if (OwnerUnit)
        {
            OwnerUnit->EquipItem(SlotType, ItemData);
        }

        RefreshEquipSlotView();
        return true;  // ドロップを処理した！
    }

    return false; // 他のWidgetに処理を渡す
}
