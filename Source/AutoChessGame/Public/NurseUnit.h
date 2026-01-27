// NurseUnit.h

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "NurseUnit.generated.h"

UCLASS()
class AUTOCHESSGAME_API ANurseUnit : public AUnit
{
    GENERATED_BODY()

public:
    ANurseUnit();

    virtual void BeginPlay() override;

    // 通常攻撃 = ヒール発動トリガー
    virtual void AttackTarget(AUnit* Target) override;

    // Skill システム経由ではもう使わない
    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

protected:
    // ★ ヒールモーション用モンタージュ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* HealMontage;

    // ★ このヒールで回復する予定の味方
    UPROPERTY()
    AUnit* PendingHealTarget = nullptr;

    // 回復対象を探す（HP割合が一番低い味方）
    AUnit* FindLowestHpAlly() const;

    // 実際の回復処理
    void ApplyHeal(AUnit* Ally);

    virtual AUnit* ChooseTarget()const override;

public:
    // ★ AnimNotify から呼ぶ
    UFUNCTION(BlueprintCallable, Category = "Heal")
    void HandleHealNotify();
};
