#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "WizardUnit.generated.h"

/**
 * 魔法攻撃が得意なユニット
 */
UCLASS()
class AUTOCHESSGAME_API AWizardUnit : public AUnit
{
    GENERATED_BODY()

public:
    AWizardUnit();

protected:
    virtual void BeginPlay() override;

public:
    // 通常攻撃もするが、スキルで魔法ダメージを与える
    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;
    virtual void AttackTarget(AUnit* Target) override;
};
