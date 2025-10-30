// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerManager.h"

// Sets default values
APlayerManager::APlayerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}
bool APlayerManager::SpendGold(int32 Amount)
{
    if (Gold < Amount) return false;
    Gold -= Amount;
    return true;
}

void APlayerManager::AddItem(const FItemData& NewItem)
{
    OwnedItems.Add(NewItem);
    UE_LOG(LogTemp, Log, TEXT("Item added: %s"), *NewItem.ItemName.ToString());
}