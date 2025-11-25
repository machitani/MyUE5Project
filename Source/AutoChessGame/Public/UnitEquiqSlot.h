#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquiqSlotType.h"
#include "ItemData.h"
#include "UnitEquiqSlot.generated.h"

class UDragDropOperation;    // ← これが大事！
class AUnit;

UCLASS()
class AUTOCHESSGAME_API UUnitEquipSlot : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip")
    AUnit* OwnerUnit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip")
    E_EquiqSlotType SlotType;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
    class UImage* ItemIcon;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
    class UTextBlock* ItemNameText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip")
    FItemData ItemData;

    void RefreshEquipSlotView();

protected:
    // UUserWidgetの正しいオーバーライドシグネチャ
    virtual bool NativeOnDrop(
        const FGeometry& InGeometry,
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation
    ) override;
};
