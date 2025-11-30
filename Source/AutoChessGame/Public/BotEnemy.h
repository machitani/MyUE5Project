// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "BotEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API ABotEnemy : public AUnit
{
	GENERATED_BODY()
	
public:
    ABotEnemy();

protected:
    virtual void BeginPlay() override;

public:
    // 必要になったらボット専用の攻撃処理に差し替え
    virtual void AttackTarget(AUnit* Target) override;
};
