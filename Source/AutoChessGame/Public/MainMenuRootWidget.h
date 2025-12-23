#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuRootWidget.generated.h"

class UTitleMenuWidget;
class UStageSelectWidget;

UCLASS()
class AUTOCHESSGAME_API UMainMenuRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void BackToTitle();

private:
    // ★Designerに置いた子を参照する（これが重要）
    UPROPERTY(meta = (BindWidget))
    UTitleMenuWidget* TitleWidget = nullptr;

    UPROPERTY(meta = (BindWidget))
    UStageSelectWidget* StageSelectWidget = nullptr;

    UFUNCTION()
    void HandleFlipHalf(bool bReverse);

    UFUNCTION()
    void HandleFlipFinished(bool bReverse);
};
