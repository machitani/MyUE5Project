#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

// ★ DataTableに使う構造体。各アイテムの基本情報。
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY()

public:

    // アイテムの内部ID（例："ATK_UP"）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FName ItemID;

    // 表示名（プレイヤーが見る名前）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FText ItemName;

    // 説明文（アイテム効果のテキスト）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FText Description;

    // ショップでの購入コスト
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    int32 Cost = 1;

    // レアリティ（1=ノーマル、2=レア、3=SR、4=URなど）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    int32 Rarity = 1;

    // アイコン画像（UMGで表示）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    UTexture2D* Icon = nullptr;
};
