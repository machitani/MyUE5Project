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

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* UnitMesh;

    /** åªç›Ç¢ÇÈÉ^ÉCÉã */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    ATile* CurrentTile;
};
