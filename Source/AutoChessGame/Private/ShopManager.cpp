#include "ShopManager.h"

AShopManager::AShopManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AShopManager::GenerateShopItems()
{
    CurrentShopItems.Empty();

    // 仮アイテム4つ
    for (int i = 0; i < 4; i++)
    {
        FShopItemData NewItem;
        NewItem.ItemName = FText::FromString(FString::Printf(TEXT("Item %d"), i));
        NewItem.Cost = (i + 1) * 3; // 3,6,9,12 とか
        NewItem.Icon = nullptr; // 後でセット

        CurrentShopItems.Add(NewItem);
    }
}
