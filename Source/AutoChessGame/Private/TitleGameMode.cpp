#include "TitleGameMode.h"
#include "TitlePlayerController.h"
#include "GameFramework/SpectatorPawn.h"

ATitleGameMode::ATitleGameMode()
{
    PlayerControllerClass = ATitlePlayerController::StaticClass();
    DefaultPawnClass = ASpectatorPawn::StaticClass(); // タイトルでは操作キャラ不要なので
}
