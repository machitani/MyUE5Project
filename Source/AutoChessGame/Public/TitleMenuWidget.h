#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleMenuWidget.generated.h"

class UButton;
class UWidgetAnimation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlipHalf, bool, bReverse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlipFinished, bool, bReverse);

UCLASS()
class AUTOCHESSGAME_API UTitleMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    /** Playボタンから呼ぶ（Forward） */
    UFUNCTION()
    void HandlePlayClicked();

    /** 外から呼ぶ（Reverse） */
    UFUNCTION(BlueprintCallable)
    void PlayFlipReverse();

    /** 外から呼ぶ（Forwardを直接呼びたい時用・任意） */
    UFUNCTION(BlueprintCallable)
    void PlayFlipForward();

    /** 半分タイミング通知（Forward/Reverse） */
    UPROPERTY(BlueprintAssignable, Category = "Flip")
    FOnFlipHalf OnFlipHalf;

    /** 終了通知（Forward/Reverse） */
    UPROPERTY(BlueprintAssignable, Category = "Flip")
    FOnFlipFinished OnFlipFinished;

protected:
    // --- BindWidget ---
    UPROPERTY(meta = (BindWidget))
    UButton* PlayButton = nullptr;

    // --- BindWidgetAnim ---
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FlipAnim = nullptr;

private:
    // 半分タイマー
    FTimerHandle FlipHalfTimer;

    // 半分通知（タイマー呼び出し用）
    void ScheduleHalfTimer(float HalfTime);

    UFUNCTION()
    void BroadcastFlipHalfForward();

    UFUNCTION()
    void BroadcastFlipHalfReverse();

    // 終了通知（アニメ終了時）
    UFUNCTION()
    void HandleFlipForwardFinished();

    UFUNCTION()
    void HandleFlipReverseFinished();

    // 現在方向
    bool bIsPlayingReverse = false;
};
