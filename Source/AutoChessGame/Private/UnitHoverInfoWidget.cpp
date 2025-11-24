#include "UnitHoverInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UUnitHoverInfoWidget::SetUnitInfo(const FName& Name, float HP, float ATK, float DEF, float MAG, float MR, const TArray<FItemData>& Items)
{
    if (UnitNameText)
        UnitNameText->SetText(FText::FromName(Name));

    if (HPText)
        HPText->SetText(FText::FromString(FString::Printf(TEXT("HP: %.0f"), HP)));

    if (AttackText)
    {
        AttackText->SetText(FText::FromString(FString::Printf(TEXT("ATK: %.0f"), ATK)));
    }

    if (DefenceText)
    {
        DefenceText->SetText(FText::FromString(FString::Printf(TEXT("DEF: %.0f"), DEF)));
    }
    
    if (MagicPowerText)
    {
        MagicPowerText->SetText(FText::FromString(FString::Printf(TEXT("MAG:%.0f"), MAG)));
    }

    if (MagicDefenceText)
    {
        MagicDefenceText->SetText(FText::FromString(FString::Printf(TEXT("MR:%.0f"), MR)));
    }

    if (ItemList)
    {
        ItemList->ClearChildren();

        for (const FItemData& Item : Items)
        {
            UTextBlock* ItemLabel = NewObject<UTextBlock>(ItemList);
            ItemLabel->SetText(FText::FromString(Item.Name.ToString()));
            ItemList->AddChild(ItemLabel);
        }
    }
}
