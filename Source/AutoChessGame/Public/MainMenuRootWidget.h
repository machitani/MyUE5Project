// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuRootWidget.generated.h"

class UTitleMenuWidget;
class UUserWidget;

UCLASS()
class AUTOCHESSGAME_API UMainMenuRootWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void ShowStageSelect();

    UFUNCTION()
    void FinishFlip();

public:
    // BPÇ≈éwíËÅiTitle/StageÇÃWidget BPÅj
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu")
    TSubclassOf<UTitleMenuWidget> TitleWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu")
    TSubclassOf<UUserWidget> StageSelectWidgetClass;

protected:
    UPROPERTY()
    UTitleMenuWidget* TitleWidget = nullptr;

    UPROPERTY()
    UUserWidget* StageSelectWidget = nullptr;
};
