// trashEnemy.h

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "trashEnemy.generated.h"

UCLASS()
class AUTOCHESSGAME_API ATrashEnemy : public AUnit
{
    GENERATED_BODY()

public:
    ATrashEnemy();

    virtual void BeginPlay() override;

    // ★ 通常攻撃を「モーション＋Notify」で行う
    virtual void AttackTarget(AUnit* Target) override;

    // Skill システムは使わない
    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

protected:
    // ★ 攻撃アニメ用モンタージュ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;

    // ★ この攻撃で殴る予定のターゲット
    UPROPERTY()
    AUnit* PendingTarget = nullptr;

    // 実際の近接ダメージ処理
    void ApplyMeleeDamage(AUnit* Target);

public:
    // ★ AnimNotify から呼ぶ
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void HandleMeleeHitNotify();
};
