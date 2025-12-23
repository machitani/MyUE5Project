#include "MainMenuRootWidget.h"
#include "TitleMenuWidget.h"
#include "StageSelectWidget.h"

void UMainMenuRootWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("[Root] NativeConstruct"));

    if (!TitleWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("[Root] TitleWidget is NULL (BindWidget failed)"));
        return;
    }
    if (!StageSelectWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("[Root] StageSelectWidget is NULL (BindWidget failed)"));
        return;
    }

    // 初期状態：StageSelect隠す、Title見せる
    TitleWidget->SetVisibility(ESlateVisibility::Visible);
    StageSelectWidget->SetVisibility(ESlateVisibility::Hidden);

    // イベント接続
    TitleWidget->OnFlipHalf.AddDynamic(this, &UMainMenuRootWidget::HandleFlipHalf);
    TitleWidget->OnFlipFinished.AddDynamic(this, &UMainMenuRootWidget::HandleFlipFinished);

    StageSelectWidget->OnBackRequested.AddDynamic(this, &UMainMenuRootWidget::BackToTitle);

    UE_LOG(LogTemp, Warning, TEXT("[Root] Bound OK"));
}

void UMainMenuRootWidget::HandleFlipHalf(bool bReverse)
{
    if (!StageSelectWidget) return;

    if (bReverse)
    {
        StageSelectWidget->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        StageSelectWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void UMainMenuRootWidget::HandleFlipFinished(bool bReverse)
{
    if (!TitleWidget || !StageSelectWidget) return;

    if (bReverse)
    {
        TitleWidget->SetVisibility(ESlateVisibility::Visible);
        StageSelectWidget->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        TitleWidget->SetVisibility(ESlateVisibility::Hidden);
        StageSelectWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void UMainMenuRootWidget::BackToTitle()
{
    UE_LOG(LogTemp, Warning, TEXT("[Root] BackToTitle called"));

    if (!TitleWidget || !StageSelectWidget) return;

    TitleWidget->SetVisibility(ESlateVisibility::Visible);
    StageSelectWidget->SetVisibility(ESlateVisibility::Visible);

    TitleWidget->PlayFlipReverse();
}
