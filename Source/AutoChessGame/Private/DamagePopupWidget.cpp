#include "DamagePopupWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UDamagePopupWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 生成されたら寿命タイマーをセット
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            LifeTimerHandle,
            this,
            &UDamagePopupWidget::HandleLifeTimeFinished,
            LifeTime,
            false
        );
    }

    // アニメがあるならここで自動再生
    if (PopupAnim)
    {
        PlayAnimation(PopupAnim);
    }
}

void UDamagePopupWidget::SetupDamage(float DamageAmount, bool bIsMagicDamage, bool bIsCritical)
{
    if (!DamageText) return;

    if (bIsCritical)
    {
        DamageText->SetText(
            FText::FromString(FString::Printf(TEXT("CRIT %.0f"), DamageAmount))
        );
        DamageText->SetColorAndOpacity(CritColor);
    }
    else
    {
        DamageText->SetText(
            FText::FromString(FString::Printf(TEXT("%.0f"), DamageAmount))
        );

        FLinearColor ColorToUse = bIsMagicDamage ? MagicColor : PhysicalColor;
        DamageText->SetColorAndOpacity(ColorToUse);
    }
}

void UDamagePopupWidget::SetupHeal(float HealAmount)
{
    if (DamageText)
    {
        // 「+50」みたいにプラスを付ける
        DamageText->SetText(
            FText::FromString(FString::Printf(TEXT("+%.0f"), HealAmount))
        );

        DamageText->SetColorAndOpacity(HealColor);
    }
}

void UDamagePopupWidget::HandleLifeTimeFinished()
{
    // ビューポートから外して自分を消す
    RemoveFromParent();
}
