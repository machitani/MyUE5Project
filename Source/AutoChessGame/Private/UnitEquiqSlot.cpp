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

        ItemData = BenchSlot->ItemData;

        if (OwnerUnit)
        {
            UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] OwnerUnit=%s Team=%d"),
                *OwnerUnit->GetName(), (int32)OwnerUnit->Team);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] OwnerUnit is NULL!!"));
        }

        if (OwnerUnit && OwnerUnit->Team == EUnitTeam::Player)
        {
            UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Call EquipItem for %s"),
                *OwnerUnit->GetName());

            OwnerUnit->EquipItem(SlotType, ItemData);
            RefreshEquipSlotView();

            // ここでベンチの見た目も消すなら
            // BenchSlot->ClearSlot();

            return true;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[EquipSlot] Cast UItemBenchSlot FAILED"));
    }

    return false;
}
