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

void AShopManager::BuyItem(FText ItemName, int32 Price)
{
    if (PlayerGold < Price)return;
    
        PlayerGold -= Price;

        UE_LOG(LogTemp, Warning, TEXT("Buy Success! Now Gold:%d"), PlayerGold);

        if (ShopWidget)
        {
            // 1. アイテムをベンチに追加
            const FItemData* FoundItem = ShopWidget->ItemTable->FindRow<FItemData>(FName(*ItemName.ToString()), TEXT(""));
            if (FoundItem)
            {
                ShopWidget->BenchItems.Add(*FoundItem);
            }

            // 2. UI 更新
            ShopWidget->UpdateGold(PlayerGold);
            ShopWidget->RefreshSlots();
            ShopWidget->RefreshItemBench();
        }
    
}


void AShopManager::PaidReroll(int32 ItemCount)
{
    const int32 RerollCost = 1;

    if (!ShopWidget) return;                       // ← 追加: 念のため
    if (PlayerGold < RerollCost)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough gold to Reroll"));
        return;
    }

    PlayerGold -= RerollCost;
    ShopWidget->UpdateGold(PlayerGold);

    RerollShop(ItemCount);                         // 無料版を呼ぶ
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
    if (!ShopWidget || !ShopWidget->ItemTable) return;

    TArray<FName> RowNames = ShopWidget->ItemTable->GetRowNames();
    if (RowNames.Num() == 0 || ItemCount <= 0)     // ← 追加: 0件安全
    {
        ShopWidget->ItemBox->ClearChildren();
        ShopWidget->RefreshSlots();
        return;
    }

    CurrentItems.Empty();
    for (int32 i = 0; i < ItemCount; i++)
    {
        const int32 Index = FMath::RandRange(0, RowNames.Num() - 1);
        if (const FItemData* Item = ShopWidget->ItemTable->FindRow<FItemData>(RowNames[Index], TEXT("Reroll")))
        {
            CurrentItems.Add(*Item);
        }
    }

    ShopWidget->UpdateShopUI();
    ShopWidget->RefreshSlots();
}
