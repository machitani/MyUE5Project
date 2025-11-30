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

public:
    // サポート用のスキル判定・実行
    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

    // 必要なら攻撃処理をカスタム
    virtual void AttackTarget(AUnit* Target) override;

    // 回復量・回復範囲
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Support")
    float HealAmount = 25.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Support")
    float HealRadius = 600.f;
};
