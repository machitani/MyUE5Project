#include "ItemHoverInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UItemHoverInfoWidget::SetItemInfo(const FItemData& ItemData)
{
    if (ItemNameText)
    {
        ItemNameText->SetText(ItemData.Name);
    }

    if (PriceText)
    {
        const FString PriceStr = FString::Printf(TEXT("Price: %d"), ItemData.Price);
        PriceText->SetText(FText::FromString(PriceStr));
    }

    if (EffectText)
    {
        FString EffectStr;

        if (ItemData.EffectType == "HP")
        {
            EffectStr = FString::Printf(TEXT("HP +%.0f"), ItemData.EffectValue);
        }
        else if (ItemData.EffectType == "Attack")
        {
            EffectStr = FString::Printf(TEXT("ATTACK +%.0f"), ItemData.EffectValue);
        }
        else
        {
            EffectStr = FString::Printf(TEXT("%s +%.0f"),
                *ItemData.EffectType, ItemData.EffectValue);
        }

        EffectText->SetText(FText::FromString(EffectStr));
    }

    if (RarityText)
    {
        FString RarityStr;
        switch (ItemData.Rarity)
        {
        case EItemRarity::Common: RarityStr = TEXT("Common"); break;
        case EItemRarity::Rare:   RarityStr = TEXT("Rare");   break;
        case EItemRarity::Epic:   RarityStr = TEXT("Epic");   break;
        default:                  RarityStr = TEXT("Unknown"); break;
        }
        RarityText->SetText(FText::FromString(RarityStr));
    }

   
}
