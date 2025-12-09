#include "DamagePopupWidget.h"
#include "Components/TextBlock.h"

void UDamagePopupWidget::SetupDamage(float DamageAmount, bool bIsMagicDamage)
{
    if (DamageText)
    {
        DamageText->SetText(
            FText::FromString(FString::Printf(TEXT("%.0f"), DamageAmount))
        );

        FSlateColor ColorToUse = bIsMagicDamage ? MagicColor : PhysicalColor;
        DamageText->SetColorAndOpacity(ColorToUse);
    }

    // アニメーションがあれば再生
    if (PopupAnim)
    {
        PlayAnimation(PopupAnim);
    }
}
