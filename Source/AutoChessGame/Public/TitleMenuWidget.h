#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleMenuWidget.generated.h"

class UWidgetSwitcher;
class UButton; 
class UWidgetAnimation;

DECLARE_MULTICAST_DELEGATE(FOnTitleFlipHalf);
DECLARE_MULTICAST_DELEGATE(FOnTitleFlipFinished);

UCLASS()
class AUTOCHESSGAME_API UTitleMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // ルート側が拾う用（StageSelectを出すタイミング）
    FOnTitleFlipHalf OnFlipHalf;
    FOnTitleFlipFinished OnFlipFinished;

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void HandlePlayClicked();

    UFUNCTION()
    void HandleFlipAnimFinished();

    void BroadcastFlipHalf();

protected:
    // BP側にあるPlayボタン（名前一致させる）
    UPROPERTY(meta = (BindWidget))
    UButton* PlayButton = nullptr;

    // BPのアニメ（名前一致させる：FlipAnim とか）
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FlipAnim = nullptr;

    FTimerHandle FlipHalfTimer;


};
