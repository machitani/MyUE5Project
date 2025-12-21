#include "TitlePlayerController.h"
#include "Blueprint/UserWidget.h"

void ATitlePlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;

    if (!TitleWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[TitlePC] TitleWidgetClass is NULL"));
        return;
    }

    TitleWidgetInstance = CreateWidget<UUserWidget>(this, TitleWidgetClass);
    if (TitleWidgetInstance)
    {
        TitleWidgetInstance->AddToViewport(100);

        FInputModeUIOnly Mode;
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(Mode);
    }
}
