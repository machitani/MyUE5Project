#include "StageSelectWidget.h"

#include "Kismet/GameplayStatics.h"
#include "TMAGameInstance.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UStageSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("[StageSelect] NativeConstruct class=%s"), *GetClass()->GetName());

    if (!BackButton)
    {
        UE_LOG(LogTemp, Error, TEXT("[StageSelect] BackButton is NULL (BindWidget failed)"));
        return;
    }

    if (Border_StageInfo)
    {
        Border_StageInfo->SetVisibility(ESlateVisibility::Hidden);
    }

    BackButton->OnClicked.AddDynamic(this, &UStageSelectWidget::HandleBackClicked);
    UE_LOG(LogTemp, Warning, TEXT("[StageSelect] BackButton bound OK"));
}


void UStageSelectWidget::ApplyStage1()
{
    if (Text_StageName) Text_StageName->SetText(FText::FromString(TEXT("STAGE:1")));
    if (Text_Waves)     Text_Waves->SetText(FText::FromString(TEXT("Wave:12")));
    if (Text_Difficulty)Text_Difficulty->SetText(FText::FromString(TEXT("EASY")));
}

void UStageSelectWidget::ApplyStage2()
{
    if (Text_StageName) Text_StageName->SetText(FText::FromString(TEXT("STAGE:2")));
    if (Text_Waves)     Text_Waves->SetText(FText::FromString(TEXT("Wave:13")));
    if (Text_Difficulty)Text_Difficulty->SetText(FText::FromString(TEXT("NORMAL")));
}

void UStageSelectWidget::ApplyStage3()
{
    if (Text_StageName) Text_StageName->SetText(FText::FromString(TEXT("STAGE:3")));
    if (Text_Waves)     Text_Waves->SetText(FText::FromString(TEXT("Wave:15")));
    if (Text_Difficulty)Text_Difficulty->SetText(FText::FromString(TEXT("HARD")));
}

void UStageSelectWidget::ShowStageInfo(int32 StageIndex)
{
    if (!Border_StageInfo) return;

    switch (StageIndex)
    {
    case 1: ApplyStage1(); break;
    case 2: ApplyStage2(); break;
    case 3: ApplyStage3(); break;
    default:
        HideStageInfo();
        return;
    }

    Border_StageInfo->SetVisibility(ESlateVisibility::Visible);
}

void UStageSelectWidget::HideStageInfo()
{
    if (Border_StageInfo)
    {
        Border_StageInfo->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UStageSelectWidget::SelectStage(int32 StageIndex)
{
    SelectedStageIndex = StageIndex;

    if (UTMAGameInstance* GI = GetGameInstance<UTMAGameInstance>())
    {
        GI->SelectedStageIndex = StageIndex;
    }
}

void UStageSelectWidget::OnStartClicked()
{
    // Šù‘¶•ûj‚Ì‚Ü‚Üi—áj
    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->ConsoleCommand(TEXT("StartGame"));
    }
}

void UStageSelectWidget::HandleBackClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("[StageSelect] Back clicked! bound=%d"),
        OnBackRequested.IsBound() ? 1 : 0);

    OnBackRequested.Broadcast();
}
