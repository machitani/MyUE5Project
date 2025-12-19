#include "TitlePlayerController.h"
#include "TitleMenuWidget.h"

void ATitlePlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;

    TitleMenu = CreateWidget<UTitleMenuWidget>(this, UTitleMenuWidget::StaticClass());
    UE_LOG(LogTemp, Warning, TEXT("[TitlePC] CreateWidget=%s"), TitleMenu ? *TitleMenu->GetName() : TEXT("NULL"));

    if (TitleMenu)
    {
        TitleMenu->AddToViewport(100);
        UE_LOG(LogTemp, Warning, TEXT("[TitlePC] AddedToViewport"));

        // ★フォーカス先を指定しない（非フォーカスWidgetに当ててエラーになるのを回避）
        FInputModeUIOnly Mode;
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(Mode);
    }
}
