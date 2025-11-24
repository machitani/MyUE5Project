#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "NurseUnit.generated.h"

/**
 * 味方を回復するヒーラーユニット（ナース）
 */
UCLASS()
class AUTOCHESSGAME_API ANurseUnit : public AUnit
{
    GENERATED_BODY()

public:
    ANurseUnit();

protected:
    virtual void BeginPlay() override;

public:
    // スキルで回復を行う
    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;
    virtual void AttackTarget(AUnit* Target) override;
};
