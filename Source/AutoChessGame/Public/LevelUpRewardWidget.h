#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelUpRewardWidget.generated.h"

class UButton;
class UTextBlock;
class APlayerManager;

/**
 * レベルアップ時のユニット報酬UI
 * 3つの候補から1つを選ぶ
 */
UCLASS()
class AUTOCHESSGAME_API ULevelUpRewardWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // --- ボタンと名前表示用 ---
    UPROPERTY(meta = (BindWidget))
    UButton* ChoiceButton1;

    UPROPERTY(meta = (BindWidget))
    UButton* ChoiceButton2;

    UPROPERTY(meta = (BindWidget))
    UButton* ChoiceButton3;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ChoiceName1;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ChoiceName2;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ChoiceName3;

    // このUIを呼び出したプレイヤーマネージャ
    UPROPERTY()
    APlayerManager* OwnerPlayerManager;

    // 候補ユニットのID（例："Nurse","Bear","Adventurer"）
    UPROPERTY(BlueprintReadOnly, Category = "Reward")
    TArray<FName> CandidateUnitIDs;

    virtual void NativeOnInitialized() override;

    // 外から候補をセットする用
    void SetupChoices(const TArray<FName>& InUnitIDs);

protected:
    UFUNCTION()
    void OnClickChoice1();

    UFUNCTION()
    void OnClickChoice2();

    UFUNCTION()
    void OnClickChoice3();

    void HandleChoice(int32 Index);
};
