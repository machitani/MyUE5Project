#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemData.h"
#include "Components/TextBlock.h"
#include "UnitHoverInfoWidget.generated.h"

UCLASS()
class AUTOCHESSGAME_API UUnitHoverInfoWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // --- テキスト類 ---
    UPROPERTY(meta = (BindWidget))
    UTextBlock* UnitNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HPText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* AttackText;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* DefenceText;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* MagicPowerText;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* MagicDefenceText;

    // --- アイテム一覧 VerticalBox ---
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* ItemList;

public:
    // ユニット情報を書き込む関数
    void SetUnitInfo(const FName& Name, float HP, float ATK, float DEF, float MAG, float MR, const TArray<FItemData>& Items);
};
