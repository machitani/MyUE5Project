#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "KnightUnit.generated.h"

/**
 * 物理タンク寄りのナイトユニット
 */
UCLASS()
class AUTOCHESSGAME_API AKnightUnit : public AUnit
{
    GENERATED_BODY()

public:
    AKnightUnit();

protected:
    virtual void BeginPlay() override;

public:
    // 必要ならナイト専用処理に変える
    virtual void AttackTarget(AUnit* Target) override;
};
