// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "ChargerEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API AChargerEnemy : public AUnit
{
	GENERATED_BODY()

public:
    AChargerEnemy();

protected:
    virtual void BeginPlay() override;

public:
    // «—ˆƒXƒLƒ‹‚Æ‚©“ü‚ê‚é‚È‚ç‚±‚±‚ğê—pˆ—‚É
    virtual void AttackTarget(AUnit* Target) override;
};
