// AdventurerUnit.h

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "AdventurerUnit.generated.h"

class APachinkoProjectile;

UCLASS()
class AUTOCHESSGAME_API AAdventurerUnit : public AUnit
{
    GENERATED_BODY()

public:
    AAdventurerUnit();

    virtual void BeginPlay() override;

    // 通常攻撃 = パチンコ
    virtual void AttackTarget(AUnit* Target) override;

    virtual bool CanUseSkill() const override;
    virtual void UseSkill(AUnit* Target) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    TSubclassOf<APachinkoProjectile> PachinkoClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    FVector PachinkoSpawnOffset = FVector(0.f, 0.f, 80.f);

    // バランス調整用。Attack * multiplier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float PachinkoDamageMultiplier = 1.0f;

    // アニメ連携
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;

    // この攻撃で狙っている敵
    UPROPERTY()
    AUnit* PendingTarget = nullptr;

    void SpawnPachinkoShot(AUnit* Target);

public:
    // AnimNotify から呼ぶ
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void HandlePachinkoShootNotify();
};
