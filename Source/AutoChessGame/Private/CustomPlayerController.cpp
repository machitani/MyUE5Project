#include "CustomPlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

ACustomPlayerController::ACustomPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    DraggingUnit = nullptr;
}

void ACustomPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("LeftClick", IE_Pressed, this, &ACustomPlayerController::OnLeftMousePressed);
    InputComponent->BindAction("LeftClick", IE_Released, this, &ACustomPlayerController::OnLeftMouseReleased);
}

void ACustomPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (DraggingUnit && DraggingUnit->bIsDragging)
    {
        FVector MouseWorld;
        if (GetMouseWorldPosition(MouseWorld))
        {
            DraggingUnit->UpdateDrag(MouseWorld);
        }
    }
}

void ACustomPlayerController::OnLeftMousePressed()
{
    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (Hit.bBlockingHit)
    {
        AUnit* HitUnit = Cast<AUnit>(Hit.GetActor());
        if (HitUnit)
        {
            FVector MouseWorld;
            if (GetMouseWorldPosition(MouseWorld))
            {
                DraggingUnit = HitUnit;
                DraggingUnit->StartDrag(MouseWorld);
                UE_LOG(LogTemp, Warning, TEXT("Start Dragging %s"), *DraggingUnit->GetName());
            }
        }
    }
}

void ACustomPlayerController::OnLeftMouseReleased()
{
    if (DraggingUnit)
    {
        DraggingUnit->EndDrag();
        UE_LOG(LogTemp, Warning, TEXT("End Dragging %s"), *DraggingUnit->GetName());
        DraggingUnit = nullptr;
    }
}

bool ACustomPlayerController::GetMouseWorldPosition(FVector& OutWorldLocation)
{
    float MouseX, MouseY;
    if (GetMousePosition(MouseX, MouseY))
    {
        FVector WorldOrigin, WorldDirection;
        if (DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDirection))
        {
            // ínñ Ç Z=0 ïΩñ Ç∆ÇµÇƒè’ìÀåvéZ
            float T = -WorldOrigin.Z / WorldDirection.Z;
            OutWorldLocation = WorldOrigin + T * WorldDirection;
            return true;
        }
    }
    return false;
}
