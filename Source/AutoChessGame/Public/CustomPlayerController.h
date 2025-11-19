#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Tile.h"
#include "Unit.h"
#include "EquiqSlotType.h"
#include "UnitEquiqSlot.h"
#include "CustomPlayerController.generated.h"

UCLASS()
class AUTOCHESSGAME_API ACustomPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ACustomPlayerController();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUnitEquipSlot> EquipSlotClass;

protected:
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void BeginPlay()override;

private:
    bool bIsDragging;
    AUnit* SelectedUnit;

    void OnLeftMouseDown();
    void OnLeftMouseUp();
    void OnRightClick();

    void CloseAllUnitInfoWidgets();

    /** ヘルパー：スクリーン座標 -> ワールドの線上の位置（カメラ近傍） */
    bool GetMouseWorldPosition(FVector& OutWorldPos, FVector& OutWorldDir) const;

    ATile* LastHighlightedTile;

    
};
