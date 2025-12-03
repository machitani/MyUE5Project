// ArcherUnit.h

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "ArcherUnit.generated.h"

class AArrowProjectile;

UCLASS()
class AUTOCHESSGAME_API AArcherUnit : public AUnit
{
    GENERATED_BODY()

public:
    AArcherUnit();

    virtual void BeginPlay() override;

    virtual void AttackTarget(AUnit* Target) override;

    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    TSubclassOf<AArrowProjectile> ArrowClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    FVector ArrowSpawnOffset = FVector(0.f, 0.f, 80.f);

    // 必要なら「遠距離補正」用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float ArrowDamageMultiplier = 1.0f;

    // 攻撃アニメ用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;

    UPROPERTY()
    AUnit* PendingTarget = nullptr;

    void SpawnArrow(AUnit* Target);

public:
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void HandleArrowShootNotify();
};
