#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.generated.h"

class ATile;
class ABoardManager;

UCLASS()
class AUTOCHESSGAME_API ACustomPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
public:
    virtual void SetupInputComponent() override;

private:
    //void OnTileClicked(ATile* ClickedTile);

    UPROPERTY()
    ABoardManager* BoardManagerRef;
};
