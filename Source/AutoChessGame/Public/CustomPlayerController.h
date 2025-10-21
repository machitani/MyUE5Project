#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Unit.h"
#include "CustomPlayerController.generated.h"

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
    /** 現在ドラッグしているユニット */
    AUnit* DraggingUnit;

    /** 左クリック押下 */
    void OnLeftMousePressed();

    /** 左クリック解放 */
    void OnLeftMouseReleased();

    /** マウス位置をワールド座標に変換 */
    bool GetMouseWorldPosition(FVector& OutWorldLocation);
};
