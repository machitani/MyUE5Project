// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageSelectWidget.generated.h"

class UButton;
class UBorder;
class UTextBlock;

UCLASS()
class AUTOCHESSGAME_API UStageSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FText StageName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Waves = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DifficulutyStars = 1;

	UPROPERTY(BlueprintReadOnly)
	int32 SelectedStageIndex = 1;

	UFUNCTION(BlueprintCallable)
	void SelectStage(int32 StageIndex);

	UFUNCTION(BlueprintCallable)
	void OnStartClicked();

	UFUNCTION(BlueprintCallable, Category = "StageSelect")
	void ShowStageInfo(int32 StageIndex);

	UFUNCTION(BlueprintCallable, Category = "StageSelect")
	void HideStageInfo();

protected:
	UPROPERTY(meta=(BindWidget))
	UBorder* Border_StageInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_StageName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Waves;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Difficulty;

	virtual void NativeConstruct() override;

private:
	void ApplyStage1();
	void ApplyStage2();
	void ApplyStage3();
};
