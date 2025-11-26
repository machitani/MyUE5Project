#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemData.h"
#include "ShopManager.generated.h"

// Åö ëOï˚êÈåæÇæÇØ
class ABoardManager;
class APlayerManager;
class UShopWidget;

class APlayerManager;

UCLASS()
class AUTOCHESSGAME_API AShopManager : public AActor
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UShopWidget> ShopWidgetClass;

    UFUNCTION(BlueprintCallable)
    void BuyItem(FName RowName, int32 Price);

    UFUNCTION(BlueprintCallable)
    void RerollShop(int32 ItemCount = 4);

    UFUNCTION(BlueprintCallable)
    void PaidReroll(int32 ItemCount = 4);

    void BuyExp();

    void RoundClearGold();

    UFUNCTION()
    void OnRoundChanged();

    UFUNCTION(BlueprintCallable)
    void AddItemToBench(const FItemData& Item);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    int32 PlayerGold = 20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    class UShopWidget* ShopWidget;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shop")
    TArray<FItemData>CurrentItems;

    UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Shop")
    TArray<FItemData>HeldItems;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shop")
    UDataTable* ItemTable;

    UPROPERTY(BlueprintReadWrite, Category = "Shop")
    TArray<FItemData> BenchItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    APlayerManager* PlayerManagerRef;

    EItemRarity GetRandomRarityForLevel(int32 PlayerLevel)const;

    bool GetRandomItemByRarity(EItemRarity Rarity, FItemData& OutItem)const;

    FItemData CreateRandomShopItem();

};
