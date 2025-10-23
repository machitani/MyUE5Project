#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Tile.h"
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
    bool bIsDragging;
    AUnit* SelectedUnit;

    void OnLeftMouseDown();
    void OnLeftMouseUp();

    /** �w���p�[�F�X�N���[�����W -> ���[���h�̐���̈ʒu�i�J�����ߖT�j */
    bool GetMouseWorldPosition(FVector& OutWorldPos, FVector& OutWorldDir) const;

    ATile* LastHighlightedTile;
};
