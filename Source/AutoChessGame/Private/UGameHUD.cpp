#include "UGameHUD.h"

void UGameHUD::SetRound(int32 Round)
{
    if (UTextBlock* RoundText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RoundText"))))
    {
        RoundText->SetText(FText::FromString(FString::Printf(TEXT("Round: %d"), Round)));
    }
}

void UGameHUD::SetPhase(FString PhaseName)
{
    if (UTextBlock* PhaseText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PhaseText"))))
    {
        PhaseText->SetText(FText::FromString(FString::Printf(TEXT("Phase: %s"), *PhaseName)));
    }
}

void UGameHUD::SetResult(FString Result)
{
    if (UTextBlock* ResultText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ResultText"))))
    {
        ResultText->SetText(FText::FromString(Result));
    }
}
