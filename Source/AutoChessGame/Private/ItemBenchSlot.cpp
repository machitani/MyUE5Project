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
    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

    UE_LOG(LogTemp, Warning, TEXT("[BenchSlot] NativeOnDragCancelled"));

    // 自分からのドラッグじゃない時は無視（保険）
    if (InOperation && InOperation->Payload != this)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return;

    // マウス位置 → ワールドに変換
    const FVector2D ScreenPos = InDragDropEvent.GetScreenSpacePosition();
    FVector WorldOrigin, WorldDir;
    if (!PC->DeprojectScreenPositionToWorld(ScreenPos.X, ScreenPos.Y, WorldOrigin, WorldDir))
    {
        return;
    }

    // ライントレースでユニット探す
    FHitResult Hit;
    FVector TraceEnd = WorldOrigin + WorldDir * 10000.0f;

    if (World->LineTraceSingleByChannel(Hit, WorldOrigin, TraceEnd, ECC_Visibility))
    {
        if (AUnit* HitUnit = Cast<AUnit>(Hit.GetActor()))
        {
            UE_LOG(LogTemp, Warning, TEXT("[BenchSlot] Hit unit: %s"), *HitUnit->GetName());

            if (HitUnit->Team == EUnitTeam::Player)
            {
                HitUnit->EquipItem(E_EquiqSlotType::Weapon, ItemData);
                UE_LOG(LogTemp, Warning, TEXT("[BenchSlot] Equipped item to %s"), *HitUnit->GetName());
            }
        }
    }
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
