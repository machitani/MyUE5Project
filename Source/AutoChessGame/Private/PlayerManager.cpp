// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerManager.h"

// Sets default values
APlayerManager::APlayerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void APlayerManager::AddGold(int32 Amount)
{
	Gold += Amount;
}

bool APlayerManager::SpendGold(int32 Amount)
{
	if (Gold >= Amount)
	{
		Gold -= Amount;
		return true;
	}
	return false;
}

void APlayerManager::LevelUp()
{
	Level++;
}




