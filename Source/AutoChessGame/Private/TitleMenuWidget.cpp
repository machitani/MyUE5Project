#include "TitleMenuWidget.h"
#include "Components/Button.h"
#include "TimerManager.h"
#include "Animation/WidgetAnimation.h"  

void UTitleMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &UTitleMenuWidget::HandlePlayClicked);
    }
}

void UTitleMenuWidget::HandlePlayClicked()
{
    if (!FlipAnim) return;

    // まず下のStageSelectを出す「半分タイミング」を予約
    const float Duration = FlipAnim->GetEndTime();          // アニメの長さ
    const float HalfTime = FMath::Max(0.01f, Duration * 0.5f);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(FlipHalfTimer);
        World->GetTimerManager().SetTimer(
            FlipHalfTimer, this, &UTitleMenuWidget::BroadcastFlipHalf, HalfTime, false
        );
    }

    // 終了通知（C++で受ける）
    FWidgetAnimationDynamicEvent FinishedEvent;
    FinishedEvent.BindUFunction(this, FName("HandleFlipAnimFinished"));
    BindToAnimationFinished(FlipAnim, FinishedEvent);

    // 再生
    PlayAnimation(FlipAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}

void UTitleMenuWidget::BroadcastFlipHalf()
{
    OnFlipHalf.Broadcast();
}

void UTitleMenuWidget::HandleFlipAnimFinished()
{
    OnFlipFinished.Broadcast();
}
