#include "CustomGameMode.h"
#include "CustomPlayerController.h"

ACustomGameMode::ACustomGameMode()
{
    // C++�ō����CustomPlayerController��ݒ�
    PlayerControllerClass = ACustomPlayerController::StaticClass();
}
