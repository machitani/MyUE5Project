#include "CustomPlayerController.h"
//#include "BoardManager.h"
//#include "Tile.h"
#include "Kismet/GameplayStatics.h"

void ACustomPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // BoardManager���擾
    //BoardManagerRef = Cast<ABoardManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoardManager::StaticClass()));
}

void ACustomPlayerController::OnTileClicked(ATile* ClickedTile)
{
    if (BoardManagerRef)
    {
        //BoardManagerRef->HandleTileClicked(ClickedTile);
    }
}
