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
    PlayFlipForward();
}

void UTitleMenuWidget::PlayFlipForward()
{
    if (!FlipAnim) return;

    bIsPlayingReverse = false;

    // タイマー（半分）
    const float Duration = FlipAnim->GetEndTime();
    const float HalfTime = FMath::Max(0.01f, Duration * 0.5f);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(FlipHalfTimer);
        World->GetTimerManager().SetTimer(
            FlipHalfTimer,
            this,
            &UTitleMenuWidget::BroadcastFlipHalfForward,
            HalfTime,
            false
        );
    }

    // Finished 多重防止
    UnbindAllFromAnimationFinished(FlipAnim);

    // 終了通知（Forward）
    FWidgetAnimationDynamicEvent FinishedEvent;
    FinishedEvent.BindUFunction(this, FName("HandleFlipForwardFinished"));
    BindToAnimationFinished(FlipAnim, FinishedEvent);

    // 再生
    PlayAnimation(FlipAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}

void UTitleMenuWidget::PlayFlipReverse()
{
    // ★ ここで確実に原因が分かるようにログ
    if (!FlipAnim)
    {
        UE_LOG(LogTemp, Error, TEXT("[Title] FlipAnim is NULL (BindWidgetAnim failed)"));

        // ★ アニメがなくても戻れるように「擬似的に」通知を流す
        OnFlipHalf.Broadcast(true);
        OnFlipFinished.Broadcast(true);
        return;
    }

    // （以下はそのまま）
    bIsPlayingReverse = true;

    const float Duration = FlipAnim->GetEndTime();
    const float HalfTime = FMath::Max(0.01f, Duration * 0.5f);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(FlipHalfTimer);
        World->GetTimerManager().SetTimer(
            FlipHalfTimer, this, &UTitleMenuWidget::BroadcastFlipHalfReverse, HalfTime, false
        );
    }

    UnbindAllFromAnimationFinished(FlipAnim);

    FWidgetAnimationDynamicEvent FinishedEvent;
    FinishedEvent.BindUFunction(this, FName("HandleFlipReverseFinished"));
    BindToAnimationFinished(FlipAnim, FinishedEvent);

    PlayAnimation(FlipAnim, 0.f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
}

void UTitleMenuWidget::ScheduleHalfTimer(float HalfTime)
{
}

void UTitleMenuWidget::BroadcastFlipHalfForward()
{
    OnFlipHalf.Broadcast(false); // false = Forward
}

void UTitleMenuWidget::BroadcastFlipHalfReverse()
{
    OnFlipHalf.Broadcast(true); // true = Reverse
}

void UTitleMenuWidget::HandleFlipForwardFinished()
{
    OnFlipFinished.Broadcast(false);
}

void UTitleMenuWidget::HandleFlipReverseFinished()
{
    OnFlipFinished.Broadcast(true);
}
