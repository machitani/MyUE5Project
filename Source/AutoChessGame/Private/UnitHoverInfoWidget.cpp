#include "UnitHoverInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UnitEquiqSlot.h"
#include "Unit.h"
#include "ShopManager.h"
#include "ShopWidget.h"
#include "BoardManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/VerticalBoxSlot.h"

void UUnitHoverInfoWidget::SetUnitInfo(const FName& Name, float HP, float ATK, float DEF, float MAG, float MR,float RANGE,float SPE,float CRIT,float CRITDAMAGE, const TArray<FItemData>& Items)
{
    if (UnitNameText)
        UnitNameText->SetText(FText::FromName(Name));

    if (HPText)
        HPText->SetText(FText::FromString(FString::Printf(TEXT("HP: %.0f"), HP)));

    if (AttackText)
    {
        AttackText->SetText(FText::FromString(FString::Printf(TEXT("ATK: %.0f"), ATK)));
    }

    if (DefenceText)
    {
        DefenceText->SetText(FText::FromString(FString::Printf(TEXT("DEF: %.0f"), DEF)));
    }
    
    if (MagicPowerText)
    {
        MagicPowerText->SetText(FText::FromString(FString::Printf(TEXT("MAG:%.0f"), MAG)));
    }

    if (MagicDefenceText)
    {
        MagicDefenceText->SetText(FText::FromString(FString::Printf(TEXT("MR:%.0f"), MR)));
    }

    if (RangeText)
    {
        RangeText->SetText(FText::FromString(FString::Printf(TEXT("RANGE:%.0f"), RANGE)));
    }

    if (MoveSpeedText)
    {
        MoveSpeedText->SetText(FText::FromString(FString::Printf(TEXT("SPE:%.0f"), SPE)));
    }

    if (CritText)
    {
        // CritChance は 0.25f みたいな想定なので 100倍して％表示
        const float Percent = CRIT * 100.f;
        CritText->SetText(
            FText::FromString(FString::Printf(TEXT("CRIT: %.0f%%"), Percent))
        );
    }

    if (CritDamageText)
    {
        const float Percent = CRITDAMAGE * 100.f;
        CritDamageText->SetText(
            FText::FromString(FString::Printf(TEXT("CRITDAMAGE:%.0f%%"), Percent))
        );
    }

 
    if (ItemList)
    {
        ItemList->ClearChildren();

        for (const FItemData& Item : Items)
        {
            UTextBlock* ItemLabel = NewObject<UTextBlock>(ItemList);
            ItemLabel->SetText(FText::FromString(Item.Name.ToString()));
            ItemList->AddChild(ItemLabel);
        }
    }
}

void UUnitHoverInfoWidget::OnRemoveAllItemsClicked()
{
    if (!OwnerUnit)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Hover] OnRemoveAllItemsClicked: OwnerUnit is NULL"));
        return;
    }

    // 装備中アイテムが無ければ何もしない
    if (OwnerUnit->EquipedItems.Num() == 0)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Hover] OnRemoveAllItemsClicked: No items to remove on %s"),
            *OwnerUnit->GetName());
        return;
    }

    // 1) 外す対象のアイテム一覧をコピーしておく
    TArray<FItemData> ItemsToReturn = OwnerUnit->EquipedItems;

    // 2) ShopManager を取得（BoardManager経由が優先、なければ検索）
    AShopManager* ShopManager = nullptr;
    if (OwnerUnit->OwningBoardManager)
    {
        ShopManager = OwnerUnit->OwningBoardManager->ShopManagerRef;
    }
    if (!ShopManager)
    {
        ShopManager = Cast<AShopManager>(
            UGameplayStatics::GetActorOfClass(this, AShopManager::StaticClass()));
    }

    if (!ShopManager)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Hover] OnRemoveAllItemsClicked: ShopManager not found"));
    }
    else
    {
        // 3) アイテムをベンチへ戻す（共通関数経由）
        for (const FItemData& Item : ItemsToReturn)
        {
            ShopManager->AddItemToBench(Item);

            UE_LOG(LogTemp, Warning,
                TEXT("[Hover] Return %s to Bench"),
                *Item.Name.ToString());
        }
    }

    // 4) ユニット側からアイテムを全部外す（内部配列／ステータスリセット用）
    OwnerUnit->RemoveItems();
    OwnerUnit->ReapplayAllItemEffects();   // 念のため再適用

    // 5) ホバーの表示を最新状態に更新
    SetUnitInfo(
        OwnerUnit->UnitID,
        OwnerUnit->HP,
        OwnerUnit->Attack,
        OwnerUnit->Defense,
        OwnerUnit->MagicPower,
        OwnerUnit->MagicDefense,
        OwnerUnit->Range,
        OwnerUnit->MoveSpeed,
        OwnerUnit->CritChance,
        OwnerUnit->CritMultiplier,
        OwnerUnit->EquipedItems  // RemoveItems 後なので多分空配列
    );
}