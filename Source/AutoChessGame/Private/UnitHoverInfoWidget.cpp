#include "UnitHoverInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UnitEquiqSlot.h"
#include "Unit.h"
#include "ShopManager.h"
#include "ShopWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/VerticalBoxSlot.h"

void UUnitHoverInfoWidget::SetUnitInfo(const FName& Name, float HP, float ATK, float DEF, float MAG, float MR,float RANGE,float SPE, const TArray<FItemData>& Items)
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

    // 1) まず「外す対象のアイテム」をコピーしておく
    if (OwnerUnit->EquipedItems.Num() == 0)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Hover] OnRemoveAllItemsClicked: No items to remove on %s"),
            *OwnerUnit->GetName());
        return;
    }

    TArray<FItemData> ItemsToReturn = OwnerUnit->EquipedItems;

    // 2) ShopManager を探す
    AShopManager* ShopManager = Cast<AShopManager>(
        UGameplayStatics::GetActorOfClass(this, AShopManager::StaticClass()));

    if (!ShopManager)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Hover] OnRemoveAllItemsClicked: ShopManager not found"));
    }
    else
    {
        // 3) ベンチ配列に戻す
        for (const FItemData& Item : ItemsToReturn)
        {
            ShopManager->BenchItems.Add(Item);
            UE_LOG(LogTemp, Warning,
                TEXT("[Hover] Return %s to Bench"),
                *Item.Name.ToString());
        }

        // 4) ベンチUIを更新
        if (ShopManager->ShopWidget)
        {
            ShopManager->ShopWidget->RefreshItemBench();
        }
    }

    // 5) ユニット側からはアイテムを全部外す（ステータスもリセット）
    OwnerUnit->RemoveItems();

    // 6) ホバーの表示を最新状態に更新
    SetUnitInfo(
        OwnerUnit->UnitID,
        OwnerUnit->HP,
        OwnerUnit->Attack,
        OwnerUnit->Defense,
        OwnerUnit->MagicPower,
        OwnerUnit->MagicDefense,
        OwnerUnit->Range,
        OwnerUnit->MoveSpeed,
        OwnerUnit->EquipedItems
    );
}
