#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "EnemyAssaultProjectile.h"
#include "TimerManager.h"
#include "SniperEnemy.generated.h"

class UAnimMontage;
class AEnemySniperProjectile; // スナイパー弾（後で作る/既にあるならそれ）
class USkeletalMeshComponent;


UCLASS()
class AUTOCHESSGAME_API ASniperEnemy : public AUnit
{
    GENERATED_BODY()
public:
    ASniperEnemy();

    virtual void AttackTarget(AUnit* Target) override;

    // AnimNotifyから呼ぶ
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void HandleSniperShootNotify();

    FTimerHandle AttackLockHandle;

    UFUNCTION()
    void EndAttackLock();

    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat|Sniper")
    float FireDelay = 0.0f;

    FTimerHandle FireDelayHandle;

    UFUNCTION()
    void FireDelayed();

protected:
    virtual void BeginPlay() override;

    // 弾をスポーン
    void SpawnSniperBullet(AUnit* Target);

    // ===== 設定（BPで差す） =====
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Sniper")
    TSubclassOf<AEnemyAssaultProjectile> SniperBulletClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Sniper")
    float BulletDamageMultiplier = 2.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Sniper")
    FVector MuzzleOffset = FVector(60.f, 0.f, 60.f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim")
    TObjectPtr<UAnimMontage> AttackMontage;

    // ===== ランタイム =====
protected:
    UPROPERTY()
    TObjectPtr<AUnit> PendingTarget = nullptr;

  
};
