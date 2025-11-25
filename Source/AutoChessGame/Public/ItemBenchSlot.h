// ItemBenchSlot.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemData.h"
#include "ItemBenchSlot.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class AUTOCHESSGAME_API UItemBenchSlot : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FItemData ItemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool bHasItem = false;

    UFUNCTION(BlueprintCallable)
    void ClearBenchItem();

    UFUNCTION(BlueprintCallable)
    void SetBenchItem(const FItemData& NewItem);

    // Ç∑Ç≈Ç… UUserWidget åpè≥ÇµÇƒÇÈÉNÉâÉXÇÃíÜÇ…

    virtual void NativeOnDragCancelled(
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation
    ) override;


protected:
    virtual void NativePreConstruct() override;
    void RefreshBenchView();

    UPROPERTY(meta = (BindWidget))
    UImage* ItemIcon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemTextIcon;
};
