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
    UE_LOG(LogTemp, Warning, TEXT("[LevelUpReward] SetupChoices called. Num=%d"), InUnitIDs.Num());
    for (auto& ID : InUnitIDs)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LevelUpReward] Candidate: %s"), *ID.ToString());
    }

    CandidateUnitIDs = InUnitIDs;
    ApplyChoiceToUI(0, ChoiceName1, ChoiceIcon1, RoleText1);
    ApplyChoiceToUI(1, ChoiceName2, ChoiceIcon2, RoleText2);
    ApplyChoiceToUI(2, ChoiceName3, ChoiceIcon3, RoleText3);
}

void ULevelUpRewardWidget::SetImageBrush(UImage* Img, UTexture2D* Tex)
{
    if (!Img) return;

    if (!Tex)
    {
        Img->SetBrushFromTexture(nullptr);
        return;
    }

    Img->SetBrushFromTexture(Tex, true);
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



void ULevelUpRewardWidget::ApplyChoiceToUI(
    int32 Index,
    UTextBlock* NameText,
    UImage* IconImage,
    UTextBlock* RoleText)
{
    if (!CandidateUnitIDs.IsValidIndex(Index)) return;

    const FName UnitID = CandidateUnitIDs[Index];

    // まず名前は UnitID のままでも表示
    if (NameText)
        NameText->SetText(FText::FromName(UnitID));

    // DataTable が無ければここまで
    if (!UnitVisualTable) return;

    static const FString Ctx(TEXT("LevelUpReward"));
    if (const FUnitVisualData* Row = UnitVisualTable->FindRow<FUnitVisualData>(UnitID, Ctx))
    {
        // 表示名
        if (NameText && !Row->DisplayName.IsEmpty())
            NameText->SetText(Row->DisplayName);

        // アイコン
        SetImageBrush(IconImage, Row->Icon);

        // ロール
        if (RoleText)
            RoleText->SetText(Row->RoleText);
    }
    UE_LOG(LogTemp, Warning, TEXT("[LevelUpReward] UnitVisualTable=%s"),
        UnitVisualTable ? *UnitVisualTable->GetName() : TEXT("NULL"));


}
