// ItemBenchSlot.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemData.h"
#include "ItemBenchSlot.generated.h"

// 先頭の forward 宣言
class UImage;
class UTextBlock;
class UButton;   // ★ 追加

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

protected:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;   // ★ 追加

    void RefreshBenchView();

    // ★ クリック処理用の関数（Button から呼ぶ）
    UFUNCTION()
    void HandleClicked();

    UPROPERTY(meta = (BindWidget))
    UImage* ItemIcon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemTextIcon;

    // ★ ここを Button にする（BP 側に "ClickArea" って名前の Button を置く）
    UPROPERTY(meta = (BindWidget))
    UButton* ClickArea;
};

