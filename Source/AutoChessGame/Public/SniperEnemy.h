// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "SniperEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API ASniperEnemy : public AUnit
{
    GENERATED_BODY()

public:
    ASniperEnemy();

protected:
    virtual void BeginPlay() override;

public:
    // スキル判定/実行（BossEnemyと同じ形）
    virtual bool CanUseSkill() const;
    virtual void UseSkill(AUnit* Target);

    // 通常攻撃（必要なら上書き）
    virtual void AttackTarget(AUnit* Target) override;

protected:
    // スキル: スナイプショット倍率
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SnipeDamageMultiplier = 2.0f;

    // スキル: 追加射程（通常射程よりさらに遠くから撃てる用）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SnipeExtraRange = 400.f;

    // スキル: 命中後のノックバック（欲しければ）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SnipeKnockbackStrength = 0.f;
	
};
