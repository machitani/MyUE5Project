// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemData.h"
#include "ShopManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopUpdated, const TArray<FItemData>&, NewItems);

UCLASS(Blueprintable,BlueprintType)
class AUTOCHESSGAME_API AShopManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopManager();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    UDataTable* ItemDataTable;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Shop")
    TArray<FItemData> CurrentItems;

    UPROPERTY(BlueprintAssignable)
    FOnShopUpdated OnShopUpdated;

    UFUNCTION(BlueprintCallable)
    void RerollShop();

    UFUNCTION(BlueprintCallable)
    bool PurchaseItem(int32 Index, class APlayerManager* Player);

    

protected:
    virtual void BeginPlay() override;

private:
    void GenerateNewItems();
    int32 ShopSize = 4;
};
