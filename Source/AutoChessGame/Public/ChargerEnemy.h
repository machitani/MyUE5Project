// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "ChargerEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API AChargerEnemy : public AUnit
{
	GENERATED_BODY()

public:
    AChargerEnemy();

    virtual void AttackTarget(AUnit* Target) override;

    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

    virtual void BeginPlay() override;

protected:
    // 斧の振りモーション用モンタージュ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;

    // この攻撃で狙っている相手（プレイヤーユニット）
    UPROPERTY()
    AUnit* PendingTarget = nullptr;

    // 実際の近接ダメージ処理
    void ApplyMeleeDamage(AUnit* Target);

public:
    // AnimNotify から呼ぶ
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void HandleMeleeHitNotify();

};
