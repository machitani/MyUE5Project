// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "SuportEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API ASuportEnemy : public AUnit
{
	GENERATED_BODY()
	
public:
    ASuportEnemy();

protected:
    virtual void BeginPlay() override;

    // ★ 味方をターゲットにする
    virtual AUnit* ChooseTarget() const override;

    // ★ 攻撃 = 回復モンタージュ再生
    virtual void AttackTarget(AUnit* Target) override;

    // AnimNotify から呼ぶ
    UFUNCTION(BlueprintCallable)
    void HandleHealNotify();

    void ApplyHeal(AUnit* Ally);

    // 回復対象
    UPROPERTY()
    AUnit* PendingHealTarget = nullptr;

public:
    // 調整用
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Support")
    float HealAmount = 25.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Support")
    float HealRadius = 10000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Support")
    UAnimMontage* HealMontage = nullptr;
};
