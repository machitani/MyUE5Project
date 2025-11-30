// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "TankEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API ATankEnemy : public AUnit
{
	GENERATED_BODY()
	
public:
    ATankEnemy();

protected:
    virtual void BeginPlay() override;

public:
    // 必要になったらタンク専用攻撃に差し替え
    virtual void AttackTarget(AUnit* Target) override;
};
