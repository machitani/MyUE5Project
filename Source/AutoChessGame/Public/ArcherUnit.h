#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "ArcherUnit.generated.h"

/**
 * ’·Ë’ö‚Ì•¨—DPS
 */
UCLASS()
class AUTOCHESSGAME_API AArcherUnit : public AUnit
{
    GENERATED_BODY()

public:
    AArcherUnit();

protected:
    virtual void BeginPlay() override;

public:
    virtual void AttackTarget(AUnit* Target) override;
};
