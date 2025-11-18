#include "UnitHoverInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UUnitHoverInfoWidget::SetUnitInfo(const FName& Name, float HP, float ATK, const TArray<FItemData>& Items)
{
    if (UnitNameText)
        UnitNameText->SetText(FText::FromName(Name));

    if (HPText)
        HPText->SetText(FText::FromString(FString::Printf(TEXT("HP: %.0f"), HP)));

    if (AttackText)
        AttackText->SetText(FText::FromString(FString::Printf(TEXT("ATK: %.0f"), ATK)));

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
