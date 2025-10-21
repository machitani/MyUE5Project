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

    /** ユニットのメッシュ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
    UStaticMeshComponent* UnitMesh;

    /** 現在いるタイル */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
    ATile* CurrentTile;
};
