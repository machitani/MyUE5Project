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

    const float Percent = OwnerUnit->GetHPPercent();
    HPBar->SetPercent(Percent);

    UE_LOG(LogTemp, Warning,
        TEXT("[HPBAR] %s HP=%.1f / MaxHP=%.1f  Percent=%.2f"),
        *OwnerUnit->GetName(),
        OwnerUnit->HP,
        OwnerUnit->MaxHP,
        Percent);
}
