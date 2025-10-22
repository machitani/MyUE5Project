#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.generated.h"

class AUnit;
class ATile;

UCLASS()
class AUTOCHESSGAME_API ACustomPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ACustomPlayerController();

protected:
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    bool bIsDragging;
    AUnit* SelectedUnit;

    void OnLeftMouseDown();
    void OnLeftMouseUp();

    /** ヘルパー：スクリーン座標 -> ワールドの線上の位置（カメラ近傍） */
    bool GetMouseWorldPosition(FVector& OutWorldPos, FVector& OutWorldDir) const;
};
