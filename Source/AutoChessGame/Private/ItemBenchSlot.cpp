#include "ItemBenchSlot.h"
#include "Components/Image.h"
#include "Unit.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/TextBlock.h"

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

void UItemBenchSlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    
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
