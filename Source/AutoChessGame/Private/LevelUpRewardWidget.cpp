#include "LevelUpRewardWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "PlayerManager.h"

void ULevelUpRewardWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ChoiceButton1)
        ChoiceButton1->OnClicked.AddDynamic(this, &ULevelUpRewardWidget::OnClickChoice1);

    if (ChoiceButton2)
        ChoiceButton2->OnClicked.AddDynamic(this, &ULevelUpRewardWidget::OnClickChoice2);

    if (ChoiceButton3)
        ChoiceButton3->OnClicked.AddDynamic(this, &ULevelUpRewardWidget::OnClickChoice3);
}

void ULevelUpRewardWidget::SetupChoices(const TArray<FName>& InUnitIDs)
{
    CandidateUnitIDs = InUnitIDs;

    // 名前表示（とりあえずUnitIDそのまま出す）
    if (ChoiceName1 && CandidateUnitIDs.Num() > 0)
        ChoiceName1->SetText(FText::FromName(CandidateUnitIDs[0]));

    if (ChoiceName2 && CandidateUnitIDs.Num() > 1)
        ChoiceName2->SetText(FText::FromName(CandidateUnitIDs[1]));

    if (ChoiceName3 && CandidateUnitIDs.Num() > 2)
        ChoiceName3->SetText(FText::FromName(CandidateUnitIDs[2]));
}

void ULevelUpRewardWidget::OnClickChoice1()
{
    HandleChoice(0);
}

void ULevelUpRewardWidget::OnClickChoice2()
{
    HandleChoice(1);
}

void ULevelUpRewardWidget::OnClickChoice3()
{
    HandleChoice(2);
}

void ULevelUpRewardWidget::HandleChoice(int32 Index)
{
    if (!OwnerPlayerManager) return;
    if (!CandidateUnitIDs.IsValidIndex(Index)) return;

    FName SelectedID = CandidateUnitIDs[Index];

    // PlayerManager に「このユニット選ばれたよ」と知らせる
    OwnerPlayerManager->OnRewardSelected(SelectedID);

    // UIを閉じる
    RemoveFromParent();
}
