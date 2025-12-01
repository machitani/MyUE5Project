#include "ShopSlotWidget.h"
#include "ShopWidget.h"
#include "ShopManager.h"
#include "ItemData.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UShopSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UShopSlotWidget::HandleClicked);
    }

    // 初期状態は売り切れていない
    SetSoldOut(false);
}

void UShopSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    UE_LOG(LogTemp, Warning, TEXT("[ShopSlot] MouseEnter %s"), *RowName.ToString());

    if (OwnerShopWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopSlot] Call ShowItemHover"));
        OwnerShopWidget->ShowItemHover(CachedItemData);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[ShopSlot] OwnerShopWidget is NULL"));
    }
}

void UShopSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (OwnerShopWidget)
    {
        OwnerShopWidget->HideItemHover();
    }
}

void UShopSlotWidget::UpdateShopState()
{
    if (!ShopManagerRef) return;

    const int32 CurrentGold = ShopManagerRef->PlayerGold;
    const int32 ItemPrice = Price;

    const bool bCanAfford = (CurrentGold >= ItemPrice);
    const bool bSoldInShop = ShopManagerRef->IsItemSoldOut(RowName);

    // すでにこのショップで購入済みなら、必ず SOLD 表示に固定
    if (bSoldInShop)
    {
        SetSoldOut(true);
        return;
    }

    // ここまで来たら「未購入」

    // ボタンのON/OFF（お金足りる？）
    if (Button)
    {
        Button->SetIsEnabled(bCanAfford);
    }

    // お金足りないときだけ少し薄く
    if (RarityFrame)
    {
        FLinearColor Color = RarityFrame->ColorAndOpacity;
        Color.A = bCanAfford ? 1.0f : 0.5f;
        RarityFrame->SetColorAndOpacity(Color);
    }

    // 内部フラグも一応同期
    bIsSoldOut = false;
}

void UShopSlotWidget::RefreshItemView(const FItemData& ItemData)
{
    // リロールで新しいアイテムが入ってきた → まず見た目をリセット
    bIsSoldOut = false;

    if (Button)
    {
        Button->SetIsEnabled(true);
    }
    if (SoldOutCover)
    {
        SoldOutCover->SetVisibility(ESlateVisibility::Collapsed);
    }
    if (RarityFrame)
    {
        FLinearColor Color = RarityFrame->ColorAndOpacity;
        Color.A = 1.0f;
        RarityFrame->SetColorAndOpacity(Color);
    }

    // いつもの見た目更新
    CachedItemData = ItemData;

    if (ItemIcon && ItemData.ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(ItemData.ItemIcon);
    }
    else if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(nullptr);
    }

    if (RarityFrame)
    {
        FLinearColor Color;

        switch (ItemData.Rarity)
        {
        case EItemRarity::Common:
            Color = FLinearColor(0.1f, 0.8f, 0.2f, 1.0f);
            break;
        case EItemRarity::Rare:
            Color = FLinearColor(0.0f, 0.35f, 0.9f, 1.0f);
            break;
        case EItemRarity::Epic:
            Color = FLinearColor(0.7f, 0.2f, 0.9f, 1.0f);
            break;
        default:
            Color = FLinearColor::Black;
            break;
        }

        RarityFrame->SetColorAndOpacity(Color);
    }
}

void UShopSlotWidget::HandleClicked()
{
    if (!ShopManagerRef) return;

    // Manager 側の情報で、すでにこの RowName が売り切れなら何もしない
    if (ShopManagerRef->IsItemSoldOut(RowName))
    {
        return;
    }

    const bool bSuccess = ShopManagerRef->TryBuyItem(RowName);

    if (bSuccess)
    {
        // Manager で MarkItemSold してるので、
        // 見た目だけここで SOLD にする
        SetSoldOut(true);
    }
}

void UShopSlotWidget::SetSoldOut(bool bSold)
{
    bIsSoldOut = bSold;

    // ボタンを押せないようにする
    if (Button)
    {
        Button->SetIsEnabled(!bIsSoldOut);
    }

    // 「SOLD」カバーを表示/非表示
    if (SoldOutCover)
    {
        SoldOutCover->SetVisibility(
            bIsSoldOut ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
        );
    }

    // ちょっと暗くして「買えない感」
    if (RarityFrame)
    {
        FLinearColor Color = RarityFrame->ColorAndOpacity;
        Color.A = bIsSoldOut ? 0.5f : 1.0f;
        RarityFrame->SetColorAndOpacity(Color);
    }
}
