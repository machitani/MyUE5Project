#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "RabbitUnit.generated.h"

UCLASS()
class AUTOCHESSGAME_API ARabbitUnit : public AUnit
{
	GENERATED_BODY()
	
public:
    ARabbitUnit();

protected:
    virtual void BeginPlay() override;

public:
    // Knightと同じ入口（戦闘AIがここを呼ぶ想定）
    virtual void AttackTarget(AUnit* Target) override;

    // アニメNotifyから呼ぶ（支援発動フレーム）
    UFUNCTION(BlueprintCallable)
    void HandleSupportNotify();

private:
    void ApplyAttackBuff(AUnit* Target);
    void RemoveAttackBuff(AUnit* Target);

private:
    UPROPERTY()
    AUnit* PendingSupportTarget = nullptr;

public:
    // ===== 支援パラメータ =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Support")
    float SupportRange = 450.f;

    // 例：1.25 = 攻撃力25%UP
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Support")
    float AttackMultiplier = 1.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Support")
    float BuffDuration = 3.0f;

    // 支援用モンタージュ（無いなら AttackMontage を使い回す）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* SupportMontage = nullptr;

private:
    // 元の攻撃力を保存（初回適用時だけ）
    TMap<TWeakObjectPtr<AUnit>, float> SavedAttack;

    // バフ解除タイマー（再付与で延長できるように保持）
    TMap<TWeakObjectPtr<AUnit>, FTimerHandle> BuffTimerHandles;

    AUnit* FindClosestAlly()const;
};
