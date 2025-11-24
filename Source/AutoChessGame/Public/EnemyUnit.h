#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "EnemyUnit.generated.h"

/**
 * 汎用敵ユニット（ロボ系のベース）
 * 将来的にさらに派生して「スナイパーロボ」「ドラゴンメカ」など増やしてOK
 */
UCLASS()
class AUTOCHESSGAME_API AEnemyUnit : public AUnit
{
    GENERATED_BODY()

public:
    AEnemyUnit();

protected:
    virtual void BeginPlay() override;

public:
    virtual void AttackTarget(AUnit* Target) override;
};
