#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "BoardManager.h"
#include "Unit.generated.h"

UCLASS()
class AUTOCHESSGAME_API AUnit : public AActor
{
    GENERATED_BODY()

public:
    AUnit();

    /** Mesh */
    UPROPERTY(VisibleAnywhere, Category = "Unit")
    UStaticMeshComponent* UnitMesh;

    /** �h���b�O���t���O */
    bool bIsDragging;

    /** �h���b�O�J�n���̃}�E�X���[���h�ƃ��j�b�g�̃I�t�Z�b�g */
    FVector DragOffset;

    /** �h���b�O�J�n */
    void StartDrag(const FVector& MouseWorld);

    /** �h���b�O�I�� */
    void EndDrag();

    /** �h���b�O�X�V�i�}�E�X���[���h�ʒu��n���ČĂԁj */
    void UpdateDrag(const FVector& MouseWorld);

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Unit")
    class ATile* CurrentTile;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
    FVector OriginalLocation;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Unit")
    ABoardManager* OwningBoardManager;
};
