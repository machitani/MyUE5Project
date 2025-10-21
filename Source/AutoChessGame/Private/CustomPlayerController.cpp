#include "CustomPlayerController.h"
#include "BoardManager.h"
#include "Tile.h"
#include "Kismet/GameplayStatics.h"


void ACustomPlayerController::BeginPlay()
{
    Super::BeginPlay();
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void ACustomPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}
