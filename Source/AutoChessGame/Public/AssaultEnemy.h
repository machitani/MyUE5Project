// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "AssaultEnemy.generated.h"


class AEnemyAssaultProjectile;

UCLASS()
class AUTOCHESSGAME_API AAssaultEnemy : public AUnit
{
    GENERATED_BODY()

public:
    AAssaultEnemy();

    virtual void BeginPlay() override;

    // 通常攻撃 = アサルト弾
    virtual void AttackTarget(AUnit* Target) override;

    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    TSubclassOf<AEnemyAssaultProjectile> AssaultBulletClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    FVector MuzzleOffset = FVector(0.f, 0.f, 80.f); // 銃口位置

    // ダメージ調整用（Attack * multiplier）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float BulletDamageMultiplier = 1.0f;

    // 攻撃アニメ同期したくなった時用（今は使わなくてもOK）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;

    // この攻撃で狙ってるプレイヤーユニット
    UPROPERTY()
    AUnit* PendingTarget = nullptr;

    void SpawnAssaultBullet(AUnit* Target);

public:
    // 将来 AnimNotify から呼びたいとき用
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void HandleAssaultShootNotify();

};
