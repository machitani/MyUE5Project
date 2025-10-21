#include "CustomGameMode.h"
#include "CustomPlayerController.h"

ACustomGameMode::ACustomGameMode()
{
    // C++‚Åì‚Á‚½CustomPlayerController‚ğİ’è
    PlayerControllerClass = ACustomPlayerController::StaticClass();
}
