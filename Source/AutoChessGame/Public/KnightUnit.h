// KnightUnit.h

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "KnightUnit.generated.h"

UCLASS()
class AUTOCHESSGAME_API AKnightUnit : public AUnit
{
    GENERATED_BODY()

public:
    AKnightUnit();

    virtual void BeginPlay() override;

    // ★ 通常攻撃を「モーション再生＋Notifyでダメージ」に変える
    virtual void AttackTarget(AUnit* Target) override;

    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

protected:
    // 攻撃モーション用モンタージュ（または単体アニメでもOK）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;

    // この攻撃で狙っている相手
    UPROPERTY()
    AUnit* PendingTarget = nullptr;

    // 近接の実ダメージ処理
    void ApplyMeleeDamage(AUnit* Target);

public:
    // AnimNotify から呼ぶ
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void HandleMeleeHitNotify();
};
