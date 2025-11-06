#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopWidget.h"
#include "ShopManager.generated.h"

UCLASS()
class AUTOCHESSGAME_API AShopManager : public AActor
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UShopWidget> ShopWidgetClass;

    UFUNCTION(BlueprintCallable)
    void BuyItem(FText ItemName, int32 Price);

    UFUNCTION(BlueprintCallable)
    void RerollShop(int32 ItemCount = 4);

    UFUNCTION(BlueprintCallable)
    void PaidReroll(int32 ItemCount = 4);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    int32 PlayerGold = 20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    class UShopWidget* ShopWidget;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shop")
    TArray<FItemData>CurrentItems;

    UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Shop")
    TArray<FItemData>HeldItems;

};
