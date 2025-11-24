#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "BearUnit.generated.h"

/**
 * 高耐久のタンク系ユニット（クマ）
 */
UCLASS()
class AUTOCHESSGAME_API ABearUnit : public AUnit
{
    GENERATED_BODY()

public:
    ABearUnit();

protected:
    virtual void BeginPlay() override;

public:
    virtual void AttackTarget(AUnit* Target) override;
};
