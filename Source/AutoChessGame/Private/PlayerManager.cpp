// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerManager.h"

// Sets default values
APlayerManager::APlayerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void APlayerManager::AddItem(const FItemData& NewItem)
{
    OwnedItems.Add(NewItem);
    //UE_LOG(LogTemp, Log, TEXT("Item added: %s"), *NewItem.ItemName.ToString());
}

void APlayerManager::AddExp(int32 Amount)
{
    CurrentExp += Amount;

    const int32 MaxPlayerLevel = 7;
    const int32 MaxUnitsHardCap = 8;

    while (CurrentExp >= ExpToNextLevel&&PlayerLevel<MaxPlayerLevel)
    {
        CurrentExp -= ExpToNextLevel;
        PlayerLevel++;

        ExpToNextLevel += 4;

        if (MaxUnitCount < MaxUnitsHardCap)
        {
            MaxUnitCount++;
        }

        UE_LOG(LogTemp, Warning, TEXT("LEVEL UP! Level: %d"), PlayerLevel);
    }

    if (PlayerLevel >= MaxPlayerLevel)
    {
        CurrentExp = 0;
    }
}

