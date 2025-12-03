#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "BearUnit.generated.h"

/**
 * 高耐久のタンク系ユニット（クマ）
 */
UCLASS()
class AUTOCHESSGAME_API ABearUnit : public AUnit
{
    GENERATED_BODY()

public:
    ABearUnit();
    virtual void BeginPlay() override;

    // 通常攻撃を「モーション＋Notify」でやる
    virtual void AttackTarget(AUnit* Target) override;

    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

protected:
    // 攻撃アニメ用モンタージュ（熊パンチ/ひっかきモーション）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;

    // この攻撃で狙っている相手
    UPROPERTY()
    AUnit* PendingTarget = nullptr;

    // 実際の近接ダメージ処理
    void ApplyMeleeDamage(AUnit* Target);

public:
    // AnimNotify から呼ぶ
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void HandleMeleeHitNotify();
};
