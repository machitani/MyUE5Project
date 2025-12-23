#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageSelectWidget.generated.h"

class UBorder;
class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackRequested);

UCLASS()
class AUTOCHESSGAME_API UStageSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    // Hover表示用（あなたの既存のまま）
    UFUNCTION(BlueprintCallable)
    void ShowStageInfo(int32 StageIndex);

    UFUNCTION(BlueprintCallable)
    void HideStageInfo();

    UFUNCTION(BlueprintCallable)
    void SelectStage(int32 StageIndex);

    UFUNCTION(BlueprintCallable)
    void OnStartClicked();

    // 戻る要求（Rootが受け取る）
    UPROPERTY(BlueprintAssignable, Category = "StageSelect")
    FOnBackRequested OnBackRequested;

protected:
    // --- BindWidget ---
    UPROPERTY(meta = (BindWidget))
    UBorder* Border_StageInfo = nullptr;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_StageName = nullptr;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_Waves = nullptr;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_Difficulty = nullptr;

    UPROPERTY(meta = (BindWidget))
    UButton* BackButton = nullptr;

private:
    UFUNCTION()
    void HandleBackClicked();

    void ApplyStage1();
    void ApplyStage2();
    void ApplyStage3();

    int32 SelectedStageIndex = 1;
};
