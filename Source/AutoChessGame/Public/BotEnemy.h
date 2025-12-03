// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "BotEnemy.generated.h"

class AEnemyAssaultProjectile;

UCLASS()
class AUTOCHESSGAME_API ABotEnemy : public AUnit
{
	GENERATED_BODY()
	
public:
    ABotEnemy();

    virtual void BeginPlay() override;

    // 通常攻撃 = バスタービーム
    virtual void AttackTarget(AUnit* Target) override;

    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

protected:
    // 撃ち出す弾（さっき作った敵用弾をセット）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    TSubclassOf<AEnemyAssaultProjectile> BusterBulletClass;

    // 腕バスターの位置オフセット（あとでBPで微調整）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    FVector MuzzleOffset = FVector(40.f, 0.f, 80.f);

    // ダメージ補正（= Attack * この値）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float BusterDamageMultiplier = 1.0f;

    // 攻撃アニメと同期したくなった時用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;

    // この攻撃で狙っているターゲット
    UPROPERTY()
    AUnit* PendingTarget = nullptr;

    void SpawnBusterShot(AUnit* Target);

public:
    // AnimNotify から呼ぶ用（今は使わなくてもOK）
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void HandleBusterShootNotify();
};
