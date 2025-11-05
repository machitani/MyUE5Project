#include "ShopManager.h"
#include "ShopWidget.h"
#include "Blueprint/UserWidget.h"

void AShopManager::BeginPlay()
{
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
    }
}

void AShopManager::BuyItem(FText ItemName, int32 Price)
{
    // ここでは PlayerGold を ShopManager 内にある前提で処理する
    if (PlayerGold >= Price)
    {
        PlayerGold -= Price;

        UE_LOG(LogTemp, Warning, TEXT("Bought: %s"), *ItemName.ToString());
        UE_LOG(LogTemp, Warning, TEXT("Remaining Gold: %d"), PlayerGold);

        // ショップ更新（必要なら）
        if (ShopWidget)
        {
            ShopWidget->UpdateShopUI();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("NOT ENOUGH GOLD"));
    }
}
