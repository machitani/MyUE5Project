// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "NinjaUnit.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API ANinjaUnit : public AUnit
{
	GENERATED_BODY()
	
public:
    ANinjaUnit();

protected:
    virtual void BeginPlay() override;

public:
    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

    virtual void AttackTarget(AUnit* Target) override;

    // ★ モンタージュのNotify等から呼ぶ
    UFUNCTION(BlueprintCallable)
    void HandleMeleeHitNotify();

    // ★ アサシン用：後衛優先ターゲット
    UFUNCTION(BlueprintCallable)
    AUnit* FindAssassinTarget() const;
    //TWeakObjectPtr<AUnit> PendingTarget;

    virtual AUnit* ChooseTarget()const override;


private:
    void ApplyMeleeDamage(AUnit* Target);


private:
    // スキル：突進（移動だけでOK）
    float SkillCooldown = 6.0f;
    float SkillTimer = 0.0f;
    float DashDistance = 450.0f;

    // 強すぎ防止：突進直後の硬直（任意）
    float PostDashLockTime = 0.15f;
    float AttackLockUntilTime = 0.0f;
};
