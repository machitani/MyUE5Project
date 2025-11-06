#include "ShopManager.h"
#include "ShopWidget.h"
#include "Blueprint/UserWidget.h"

void AShopManager::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("SHOP"));

    if (ShopWidgetClass)
    {
        ShopWidget = CreateWidget<UShopWidget>(GetWorld(), ShopWidgetClass);
        if (ShopWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("SHOP"));
            ShopWidget->ShopManager = this;  
            ShopWidget->AddToViewport();
            ShopWidget->UpdateShopUI();
            ShopWidget->UpdateGold(PlayerGold);
            ShopWidget->RefreshSlots();
        }
    }
}

void AShopManager::BuyItem(FText ItemName, int32 Price)
{
    if (PlayerGold >= Price)
    {
        PlayerGold -= Price;

        UE_LOG(LogTemp, Warning, TEXT("Buy Success! Now Gold:%d"), PlayerGold);

        if (ShopWidget)
        {
            ShopWidget->UpdateGold(PlayerGold);
            ShopWidget->RefreshSlots();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough Gold!"));
    }
}
