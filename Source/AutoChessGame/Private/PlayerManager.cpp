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

bool APlayerManager::BuyItem(const FShopItem& Item)
{
    if (Gold >= Item.Price)
    {
        Gold -= Item.Price;
        Inventory.Add(Item);

        // デバッグ用
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
                FString::Printf(TEXT("Bought %s! Remaining Gold: %d"), *Item.ItemName.ToString(), Gold));
        }

        return true;
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Not enough Gold!"));
        }
        return false;
    }
}


