// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "TankEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API ATankEnemy : public AUnit
{
	GENERATED_BODY()
	
public:
    ATankEnemy();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 通常攻撃 = ガード行動
    virtual void AttackTarget(AUnit* Target) override;

    // Skill は使わない
    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

protected:
    // ガードモーション用モンタージュ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* GuardMontage;

    // ガード中フラグ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Guard")
    bool bIsGuardActive = false;

    // ガード継続時間（秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guard")
    float GuardDuration = 2.0f;

    // 残りガード時間
    float GuardRemainingTime = 0.0f;

    // ガード時の防御倍率（ダメージ何割にするか）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guard")
    float GuardDamageReduceRate = 0.5f; // 0.5 = 半分ダメージ

    // 実際のダメージ計算
    void ApplyGuardedDamage(float& DamageAmount) const;

public:
    // AnimNotify から呼ぶ：この瞬間からガード状態になる
    UFUNCTION(BlueprintCallable, Category = "Guard")
    void HandleGuardStartNotify();

    // 物理・魔法ダメージをガード反映したいなら override
    virtual void TakePhysicalDamage(float DamageAmount) override;
    virtual void TakeMagicDamage(float DamageAmount) override;
};
