// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "AMissileProjectileBase.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API ABossEnemy : public AUnit
{
	GENERATED_BODY()

public:
    ABossEnemy();

protected:
    virtual void BeginPlay() override;

public:
    // ボス専用スキル判定・実行
    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

    // 必要なら攻撃処理をカスタム
    virtual void AttackTarget(AUnit* Target) override;

    // ミサイル範囲攻撃用パラメータ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
    float MissileRadius = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
    float MissileDamageMultiplier = 0.7f; // 通常攻撃の 0.7 倍を範囲に追加

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Boss|Skill")
    TSubclassOf<AAMissileProjectileBase>MissileClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Skill")
    UAnimMontage* MissileMontage = nullptr;

    UPROPERTY()
    AUnit* PendingMissileTarget = nullptr;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Boss|Skill")
    FVector MissileSpawnOffset = FVector(0.f, 0.f, 120.f);

    UFUNCTION(BlueprintCallable)
    void HandleMissileHitNotify();

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Missile")
    FName MissileMuzzleSocketName = "Muzzle";

};
