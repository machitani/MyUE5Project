// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "DuckUnit.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API ADuckUnit : public AUnit
{
	GENERATED_BODY()
	
public:
    ADuckUnit();

protected:
    virtual void BeginPlay() override;

public:
    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

    virtual void AttackTarget(AUnit* Target) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimMontage* AttackMontage = nullptr;

    // ★ 攻撃ヒット時（AnimNotifyから呼ぶ）
    UFUNCTION(BlueprintCallable)
    void HandleMeleeHitNotify();

private:
    void ApplyDuckHit(AUnit* Target);

private:
    // Knightと同じ流れ
    bool bIsAttacking = false;
    TWeakObjectPtr<AUnit> PendingTarget;

    // ★ 毒パラメータ（調整用）
    float PoisonDuration = 4.0f;
    float PoisonDamagePerTick = 3.0f;
    float PoisonTickInterval = 1.0f;

    // ★ 初撃ダメ（任意：0でもOK）
    float InitialHitDamage = 6.0f;
};
