#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleMenuWidget.generated.h"

class UWidgetSwitcher;
class UButton; 

UCLASS()
class AUTOCHESSGAME_API UTitleMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;

private:
    UPROPERTY() UWidgetSwitcher* Switcher = nullptr;

    UPROPERTY() UButton* StartButton = nullptr;
    UPROPERTY() UButton* BackButton = nullptr;
    UPROPERTY() UButton* Stage1Button = nullptr;
    UPROPERTY() UButton* Stage2Button = nullptr;
    UPROPERTY() UButton* Stage3Button = nullptr;

    UFUNCTION() void OnStartClicked();
    UFUNCTION() void OnBackClicked();
    UFUNCTION() void OnStage1Clicked();
    UFUNCTION() void OnStage2Clicked();
    UFUNCTION() void OnStage3Clicked();

    void GoToStage(int32 StageIndex);

};
