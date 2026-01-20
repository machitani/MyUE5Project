#include "ShopManager.h"
#include "ShopWidget.h"
#include "ItemBenchSlot.h"
#include "ShopSlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "PlayerManager.h"
#include "BoardManager.h"
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
   
}


void AShopManager::PaidReroll(int32 ItemCount)
{
    UE_LOG(LogTemp, Warning, TEXT("[ShopManager] PaidReroll called. Gold=%d"), PlayerGold);

    const int32 RerollCost = 1;

    if (!ShopWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("[ShopManager] PaidReroll: ShopWidget is NULL"));
        return;
    }

    if (PlayerGold < RerollCost)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopManager] Not enough gold to Reroll"));
        return;
    }

    PlayerGold -= RerollCost;
    ShopWidget->UpdateGold(PlayerGold);

    RerollShop(ItemCount);

    // ★ ここで各スロットの「買える / 買えない」状態も更新
    ShopWidget->RefreshSlots();
}

void AShopManager::BuyExp()
{
    //フェーズチェック
    if (!PlayerManagerRef || !PlayerManagerRef->BoardManagerRef)
    {
        return;
    }

    if (PlayerManagerRef->BoardManagerRef->CurrentPhase != EGamePhase::Preparation)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuyExp: Only available in Preparation phase"));
        return;
    }

    const int32 Cost = 4;
    const int32 ExpGain = 4;

    if (PlayerGold < Cost)
    {
        return;
    }

    PlayerGold -= Cost;

    if (ShopWidget)
    {
        ShopWidget->UpdateGold(PlayerGold);

        // ★ ここ追加：ゴールド変わったので「買える/買えない」色を更新
        ShopWidget->RefreshSlots();
    }

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

void AShopManager::OnRoundChanged()
{
    const int32 ItemCount = 4;

    RerollShop(ItemCount);

    if (ShopWidget)
    {
        ShopWidget->UpdateGold(PlayerGold);
        ShopWidget->RefreshSlots();
    }
}

void AShopManager::AddItemToBench(const FItemData& Item)
{
    if (BenchItems.Num() >= MaxBenchItems)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopManager] Bench is full (%d). Can't add item."), MaxBenchItems);
        return; // 追加しない
    }

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

bool AShopManager::RemoveItemFromBenchByRowName(FName RowName)
{
    for (int32 i = 0; i < BenchItems.Num(); ++i)
    {
        if (BenchItems[i].RowName == RowName)
        {
            BenchItems.RemoveAt(i);

            if (ShopWidget)
            {
                ShopWidget->RefreshItemBench();
            }

            return true;
        }
    }

    UE_LOG(LogTemp, Warning,
        TEXT("RemoveItemFromBenchByRowName: Item %s not found in bench"),
        *RowName.ToString());

    return false;
}

void AShopManager::RerollShop(int32 ItemCount)
{
    if (!ShopWidget || !ItemTable) return;

    // ★ このショップの売り切れ情報をリセット
    SoldOutItemRowNames.Empty();

    CurrentItems.Empty();

    for (int32 i = 0; i < ItemCount; i++)
    {
        FItemData NewItem = CreateRandomShopItem();
        CurrentItems.Add(NewItem);
    }

    ShopWidget->UpdateShopUI();
    ShopWidget->RefreshSlots();
}

bool AShopManager::TryBuyItem(FName RowName)
{
    if (!ItemTable) return false;

    FItemData* Item = ItemTable->FindRow<FItemData>(RowName, TEXT("Shop Buy"));
    if (!Item) return false;

    // ★ 先にベンチ上限チェック
    if (BenchItems.Num() >= MaxBenchItems)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopManager] Bench is full (%d). Can't buy."), MaxBenchItems);
        return false;
    }

    if (PlayerGold < Item->Price)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopManager] Not enough gold"));
        return false;
    }

    PlayerGold -= Item->Price;

    // ★ RowNameも保持したいならコピーしてセット（安全）
    FItemData Copy = *Item;
    // Copy.RowName = RowName;  // ItemTable側にRowNameが入ってないなら有効化

    BenchItems.Add(Copy);

    if (ShopWidget)
    {
        ShopWidget->UpdateGold(PlayerGold);
        ShopWidget->RefreshItemBench();
        ShopWidget->RefreshSlots();
    }

    UE_LOG(LogTemp, Warning, TEXT("[ShopManager] Buy %s"), *Item->Name.ToString());
    return true;
}

bool AShopManager::IsItemSoldOut(FName RowName) const
{
    return SoldOutItemRowNames.Contains(RowName);
}

void AShopManager::MarkItemSold(FName RowName)
{
    SoldOutItemRowNames.Add(RowName);
}
