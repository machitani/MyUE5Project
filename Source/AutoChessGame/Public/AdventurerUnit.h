#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "AdventurerUnit.generated.h"

/**
 * 後方からパチンコで攻撃する冒険家ユニット
 */
UCLASS()
class AUTOCHESSGAME_API AAdventurerUnit : public AUnit
{
    GENERATED_BODY()

public:
    AAdventurerUnit();

protected:
    virtual void BeginPlay() override;

public:
    // 射撃ユニットなので、将来的に弾を飛ばす処理をここで行う
    virtual void AttackTarget(AUnit* Target) override;
};
