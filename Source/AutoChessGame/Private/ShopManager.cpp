#include "ShopManager.h"
#include "ShopWidget.h"
#include "ItemBenchSlot.h"
#include "ShopSlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "PlayerManager.h"
#include "ItemData.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


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
    if (!PlayerManagerRef)
    {
        TArray<AActor*>Found;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerManager::StaticClass(), Found);
        if (Found.Num() > 0)
        {
            PlayerManagerRef = Cast<APlayerManager>(Found[0]);
        }
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

        // ★ RowName もコピーしたうえで Bench に追加
        FItemData Copy = *FoundItem;
        Copy.RowName = RowName;

        AddItemToBench(Copy);
    }

    ShopWidget->UpdateGold(PlayerGold);
    ShopWidget->RefreshSlots();
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

void AShopManager::BuyExp()
{
    const int32 Cost = 4;
    const int32 ExpGain = 4;

    if (PlayerGold < Cost)
    {
        return;
    }
    PlayerGold -= Cost;
    ShopWidget->UpdateGold(PlayerGold);

    if (PlayerManagerRef)
    {
        PlayerManagerRef->AddExp(ExpGain);
    }
}

void AShopManager::RoundClearGold()
{
    PlayerGold += 5;
    ShopWidget->UpdateGold(PlayerGold);
}

void AShopManager::AddItemToBench(const FItemData& Item)
{
    BenchItems.Add(Item);
    
        if (ShopWidget)
        {
            ShopWidget->RefreshItemBench();
        }
}

EItemRarity AShopManager::GetRandomRarityForLevel(int32 PlayerLevel) const
{
    int32 CommonRate = 80;
    int32 RareRate = 20;
    int32 EpicRate = 0;

    if (PlayerLevel >= 3 && PlayerLevel <= 4)
    {
        CommonRate = 60;
        RareRate = 30;
        EpicRate = 10;
    }
    else if (PlayerLevel >= 5 && PlayerLevel <= 6)
    {
        CommonRate = 40;
        RareRate = 40;
        EpicRate = 20;
    }
    else if (PlayerLevel >= 7)
    {
        CommonRate = 20;
        RareRate = 50;
        EpicRate = 30;
    }

    int Roll = FMath::RandRange(1, 100);
    if (Roll <= CommonRate)
    {
        return  EItemRarity::Common;
    }
    else if (Roll <= CommonRate + RareRate)
    {
        return EItemRarity::Rare;
    }
    else
    {
        return EItemRarity::Epic;
    }
}

bool AShopManager::GetRandomItemByRarity(EItemRarity Rarity, FItemData& OutItem) const
{
    if (!ItemTable) return false;

    static const FString Context(TEXT("ItemRandomPick"));
    TArray<FName> RowNames = ItemTable->GetRowNames();

    // ★ Rarity が一致する RowName だけ集める
    TArray<FName> Candidates;

    for (const FName& RowName : RowNames)
    {
        if (FItemData* Row = ItemTable->FindRow<FItemData>(RowName, Context))
        {
            if (Row->Rarity == Rarity)
            {
                Candidates.Add(RowName);
            }
        }
    }

    if (Candidates.Num() == 0)
    {
        return false;
    }

    int32 Index = FMath::RandRange(0, Candidates.Num() - 1);
    FName SelectedRowName = Candidates[Index];

    if (FItemData* Row = ItemTable->FindRow<FItemData>(SelectedRowName, Context))
    {
        OutItem = *Row;
        OutItem.RowName = SelectedRowName;   // ★ ここで RowName を覚えさせる
        return true;
    }

    return false;
}

FItemData AShopManager::CreateRandomShopItem()
{
    FItemData Result;

    if (!PlayerManagerRef || !ItemTable)
    {
        return Result; // デフォルト（全部0）返す
    }

    int32 Level = PlayerManagerRef->PlayerLevel;

    // 1. レベルからレアリティを決める
    EItemRarity Rarity = GetRandomRarityForLevel(Level);

    // 2. そのレアリティからアイテムを1つ選ぶ
    if (!GetRandomItemByRarity(Rarity, Result))
    {
        // 指定レアリティのアイテムがなかった場合 → 全レアリティからランダムに拾う fallback
        UE_LOG(LogTemp, Warning, TEXT("No items for rarity %d, fallback to any rarity."), (int32)Rarity);

        static const FString Context(TEXT("ItemFallback"));
        TArray<FName> RowNames = ItemTable->GetRowNames();
        if (RowNames.Num() > 0)
        {
            int32 Index = FMath::RandRange(0, RowNames.Num() - 1);
            FName SelectedRowName = RowNames[Index];

            if (FItemData* Row = ItemTable->FindRow<FItemData>(SelectedRowName, Context))
            {
                Result = *Row;
                Result.RowName = SelectedRowName; // ★ fallback でも RowName セット
            }
        }
    }

    UE_LOG(LogTemp, Warning,
        TEXT("Shop Item: %s (Rarity=%d, Level=%d)"),
        *Result.RowName.ToString(),
        (int32)Result.Rarity,
        PlayerManagerRef->PlayerLevel
    );

    return Result;
}

void AShopManager::RerollShop(int32 ItemCount)
{
    if (!ShopWidget || !ItemTable) return;

    CurrentItems.Empty();

    for (int32 i = 0; i < ItemCount; i++)
    {
        // ★ ここでレベルを見て、レアリティを決めて、アイテムを1つ作る
        FItemData NewItem = CreateRandomShopItem();

        // RowName が中にセットされていることが前提
        CurrentItems.Add(NewItem);
    }

    // UI更新
    ShopWidget->UpdateShopUI();

    for (int32 i = 0; i < CurrentItems.Num(); i++)
    {
        if (ShopWidget->ShopSlots.IsValidIndex(i) && ShopWidget->ShopSlots[i])
        {
            ShopWidget->ShopSlots[i]->RefreshItemView(CurrentItems[i]);
        }
    }

    ShopWidget->RefreshSlots();
}

