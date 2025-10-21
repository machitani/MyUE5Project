#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"

class ATile;

UCLASS()
class AUTOCHESSGAME_API AUnit : public AActor
{
    GENERATED_BODY()

public:
    AUnit();

    /** ���j�b�g�̃��b�V�� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
    UStaticMeshComponent* UnitMesh;

    /** ���݂���^�C�� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    ATile* CurrentTile;
};
