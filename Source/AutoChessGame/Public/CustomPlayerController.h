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
    /** ���݃h���b�O���Ă��郆�j�b�g */
    AUnit* DraggingUnit;

    /** ���N���b�N���� */
    void OnLeftMousePressed();

    /** ���N���b�N��� */
    void OnLeftMouseReleased();

    /** �}�E�X�ʒu�����[���h���W�ɕϊ� */
    bool GetMouseWorldPosition(FVector& OutWorldLocation);
};
