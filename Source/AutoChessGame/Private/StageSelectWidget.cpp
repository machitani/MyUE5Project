#include "StageSelectWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TMAGameInstance.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UStageSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 初期は非表示
    if (Border_StageInfo)
    {
        Border_StageInfo->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UStageSelectWidget::ApplyStage1()
{
    if (Text_StageName)Text_StageName->SetText(FText::FromString(TEXT("STAGE:1")));
    if (Text_Waves)Text_Waves->SetText(FText::FromString(TEXT("Wave:12")));
    if (Text_Difficulty)Text_Difficulty->SetText(FText::FromString(TEXT("EASY")));
}

void UStageSelectWidget::ApplyStage2()
{
    if (Text_StageName)Text_StageName->SetText(FText::FromString(TEXT("STAGE:2")));
    if (Text_Waves)Text_Waves->SetText(FText::FromString(TEXT("Wave:13")));
    if (Text_Difficulty)Text_Difficulty->SetText(FText::FromString(TEXT("NOMAL")));
}

void UStageSelectWidget::ApplyStage3()
{
    if (Text_StageName)Text_StageName->SetText(FText::FromString(TEXT("STAGE:3")));
    if (Text_Waves)Text_Waves->SetText(FText::FromString(TEXT("Wave:15")));
    if (Text_Difficulty)Text_Difficulty->SetText(FText::FromString(TEXT("HARD")));
}

void UStageSelectWidget::ShowStageInfo(int32 StageIndex)
{
    if (!Border_StageInfo) return;

    // ステージごとの表示内容を反映
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
    // ここではレベル遷移しない
    // 既存の「開始処理」を呼ぶだけ

    if (APlayerController* PC = GetOwningPlayer())
    {
        // 例：PlayerControllerにStartGame()がある場合
        PC->ConsoleCommand(TEXT("StartGame"));
    }
}
