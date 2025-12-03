// WizardUnit.h

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "WizardUnit.generated.h"

class AFireballProfectile;

UCLASS()
class AUTOCHESSGAME_API AWizardUnit : public AUnit
{
    GENERATED_BODY()

public:
    AWizardUnit();

    virtual void BeginPlay() override;

    // ★ 通常攻撃をファイアーボールに差し替える
    virtual void AttackTarget(AUnit* Target) override;

    // スキルは別枠で残しておく（今は未使用でもOK）
    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

    UFUNCTION(BlueprintCallable,Category="Attack")
    void HandleFireballShootNotify();

protected:
    // ファイアーボールのクラス（BP でセット）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    TSubclassOf<AFireballProfectile> FireballClass;

    // 発射処理
    void SpawnFireball(AUnit* Target);

    // 発射位置のオフセット（頭の少し前とか）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    FVector FireballSpawnOffset = FVector(0.f, 0.f, 80.f);

    // 魔力にかける倍率
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float MagicAttackMultiplier = 1.5f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Animation")
    UAnimMontage* AttackMontage;

    UPROPERTY()
    AUnit* PendingTarget = nullptr;

    
};
