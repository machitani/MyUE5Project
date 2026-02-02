// EndMenuWidget.cpp
#include "EndMenuWidget.h"
#include "Components/Image.h"

void UEndMenuWidget::SetEndTitle(bool bGameClear)
{
    if (ClearImage)
    {
        ClearImage->SetVisibility(bGameClear ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    if (GameOverImage)
    {
        GameOverImage->SetVisibility(bGameClear ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    }
}
