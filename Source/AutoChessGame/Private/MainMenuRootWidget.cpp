#include "MainMenuRootWidget.h"
#include "TitleMenuWidget.h"

void UMainMenuRootWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 下：StageSelect（最初は隠す）
    if (StageSelectWidgetClass)
    {
        StageSelectWidget = CreateWidget<UUserWidget>(GetWorld(), StageSelectWidgetClass);
        if (StageSelectWidget)
        {
            StageSelectWidget->AddToViewport(0);
            StageSelectWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // 上：Title（めくる対象）
    if (TitleWidgetClass)
    {
        TitleWidget = CreateWidget<UTitleMenuWidget>(GetWorld(), TitleWidgetClass);
        if (TitleWidget)
        {
            TitleWidget->AddToViewport(1);

            TitleWidget->OnFlipHalf.AddUObject(this, &UMainMenuRootWidget::ShowStageSelect);
            TitleWidget->OnFlipFinished.AddUObject(this, &UMainMenuRootWidget::FinishFlip);
        }
    }
}

void UMainMenuRootWidget::ShowStageSelect()
{
    if (StageSelectWidget)
    {
        StageSelectWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void UMainMenuRootWidget::FinishFlip()
{
    if (TitleWidget)
    {
        TitleWidget->RemoveFromParent(); // 完全に消す
        TitleWidget = nullptr;
    }
}
