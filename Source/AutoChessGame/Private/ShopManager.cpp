
#include "ShopManager.h"
#include "PlayerManager.h"
#include "Kismet/KismetMathLibrary.h"

AShopManager::AShopManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AShopManager::BeginPlay()
{
	Super::BeginPlay();
	GenerateNewItems();
	
}

void AShopManager::GenerateNewItems()
{
	if (!ItemDataTable)return;

	CurrentItems.Empty();

	TArray<FItemData*>ALLItems;
	ItemDataTable->GetAllRows(TEXT("Shop"), ALLItems);

	//ƒ‰ƒ“ƒ_ƒ€‚ÉShopSizeŒÂ‘I‚Ô
	for (int i = 0; i < ShopSize && ALLItems.Num()>0; i++)
	{
		int32 RandIndex = UKismetMathLibrary::RandomInteger(ALLItems.Num());
		CurrentItems.Add(*ALLItems[RandIndex]);
	}
	OnShopUpdated.Broadcast(CurrentItems);
}

void AShopManager::RerollShop()
{
	GenerateNewItems();
}

bool AShopManager::PurchaseItem(int32 Index, APlayerManager* Player)
{
	if (!Player || !CurrentItems.IsValidIndex(Index))return false;

	FItemData Item = CurrentItems[Index];
	if (Player->Gold < Item.Cost)return false;

	Player->Gold -= Item.Cost;
	Player->AddItem(Item);

	CurrentItems.RemoveAt(Index);
	OnShopUpdated.Broadcast(CurrentItems);
	
	return true;
}



