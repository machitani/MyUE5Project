// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "AssaultEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API AAssaultEnemy : public AUnit
{
	GENERATED_BODY()
	
public:
    AAssaultEnemy();

protected:
    virtual void BeginPlay() override;

public:
    // 必要ならアサルト専用の攻撃処理に変える
    virtual void AttackTarget(AUnit* Target) override;

};
