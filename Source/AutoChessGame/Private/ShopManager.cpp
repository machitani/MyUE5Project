#include "ShopManager.h"
#include "ShopWidget.h"
#include "ItemBenchSlot.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetMathLibrary.h"

void AShopManager::BeginPlay()
{
    if (!ShopWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ShopWidgetClass is NULL!!"));
    }
    Super::BeginPlay();

    if (ShopWidgetClass)
    {
        ShopWidget = CreateWidget<UShopWidget>(GetWorld(), ShopWidgetClass);
        if (ShopWidget)
        {
            ShopWidget->ShopManager = this;  
            ShopWidget->AddToViewport();
            ShopWidget->UpdateShopUI();
        }
        RerollShop(4);
        ShopWidget->UpdateGold(PlayerGold);
        ShopWidget->RefreshSlots();
    }
}

void AShopManager::BuyItem(FName RowName, int32 Price)
{
    if (PlayerGold < Price) return;
    PlayerGold -= Price;

    const FItemData* FoundItem = ItemTable->FindRow<FItemData>(RowName, TEXT(""));
    if (FoundItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("RINGO"));
        BenchItems.Add(*FoundItem);
    }

    ShopWidget->UpdateGold(PlayerGold);
    ShopWidget->RefreshSlots();
    ShopWidget->RefreshItemBench();
}


void AShopManager::PaidReroll(int32 ItemCount)
{
    const int32 RerollCost = 1;

    if (!ShopWidget) return;                       // Å© í«â¡: îOÇÃÇΩÇﬂ
    if (PlayerGold < RerollCost)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough gold to Reroll"));
        return;
    }

    PlayerGold -= RerollCost;
    ShopWidget->UpdateGold(PlayerGold);

    RerollShop(ItemCount);                         // ñ≥óøî≈ÇåƒÇ‘
}

void AShopManager::AddItemToBench(const FItemData& Item)
{
    BenchItems.Add(Item);
    
        if (ShopWidget)
        {
            ShopWidget->RefreshItemBench();
        }
}

void AShopManager::RerollShop(int32 ItemCount)
{
    if (!ShopWidget || !ItemTable) return;

    TArray<FName> RowNames = ItemTable->GetRowNames();
    if (RowNames.Num() == 0) return;

    CurrentItems.Empty();

    for (int32 i = 0; i < ItemCount; i++)
    {
        int32 Index = FMath::RandRange(0, RowNames.Num() - 1);
        FName SelectedRow = RowNames[Index];

        const FItemData* FoundItem = ItemTable->FindRow<FItemData>(SelectedRow, TEXT("Reroll"));

        if (FoundItem)
        {
            FItemData Copy = *FoundItem;
            Copy.RowName = SelectedRow;   // Å© ÅöÇ±ÇÍÇ™ç≈èdóv Åö
            CurrentItems.Add(Copy);
        }
    }

    ShopWidget->UpdateShopUI();
    ShopWidget->RefreshSlots();
}
