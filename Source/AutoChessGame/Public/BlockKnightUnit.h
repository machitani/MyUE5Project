// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "BlockKnightUnit.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API ABlockKnightUnit : public AUnit
{
	GENERATED_BODY()
	
public:
    ABlockKnightUnit();

protected:
    virtual void BeginPlay() override;

    // 一番HPが低い味方を探す
    AUnit* FindLowestHpAlly() const;

    // 定期的に支援行動を試す（敵がいなくてもOK）
    UFUNCTION()
    void TrySupport();

    // モンタージュNotifyから呼ぶ用（Rabbitと同じ思想）
    UFUNCTION()
    void HandleSupportNotify();

    void ApplyDefenseBuff(AUnit* Target);
    void RemoveDefenseBuff(AUnit* Target);

    virtual AUnit* ChooseTarget()const override;
    virtual void AttackTarget(AUnit*target)override;

protected:
    // ====== 調整パラメータ ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Support")
    float SupportRange = 320.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Support")
    float SupportTick = 0.8f;   // 支援試行間隔

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Support")
    float BuffDuration = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Support")
    float DefenseMultiplier = 1.35f; // 防御バフ倍率（例：DEF x1.35）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Support")
    UAnimMontage* SupportMontage = nullptr;

    // ====== 内部 ======
    FTimerHandle SupportLoopHandle;

    UPROPERTY()
    AUnit* PendingSupportTarget = nullptr;

    // バフ解除用：元のDefenseを保存
    TMap<TWeakObjectPtr<AUnit>, float> SavedDefense;
    TMap<TWeakObjectPtr<AUnit>, FTimerHandle> BuffTimerHandles;
};
