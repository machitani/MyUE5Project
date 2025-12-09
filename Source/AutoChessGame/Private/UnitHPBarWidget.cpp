// UnitHPBarWidget.cpp
#include "UnitHPBarWidget.h"
#include "Components/ProgressBar.h"
#include "Unit.h"

void UUnitHPBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!HPBar || !OwnerUnit)
    {
        return;
    }

    // AUnit ‚É‘‚¢‚Ä‚ ‚é GetHPPercent ‚ð‚»‚Ì‚Ü‚ÜŽg‚¤
    const float Percent = OwnerUnit->GetHPPercent();
    HPBar->SetPercent(Percent);
}
