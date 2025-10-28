// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopItem.h"
#include "PlayerManager.generated.h"

UCLASS()
class AUTOCHESSGAME_API APlayerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 Gold = 10;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Economy")
	int32 Level = 1;

	UFUNCTION(BlueprintCallable)
	void AddGold(int32 Amount);

	UFUNCTION(BlueprintCallable)
	bool SpendGold(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void LevelUp();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FShopItem>Inventory;

	UFUNCTION(BlueprintCallable)
	bool BuyItem(const FShopItem& Item);
};
