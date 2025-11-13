// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemData.h"
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

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<FItemData> OwnedItems;

	UFUNCTION(BlueprintCallable)
	void AddGold(int32 Amount) { Gold += Amount; }

	UFUNCTION(BlueprintCallable)
	bool SpendGold(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void AddItem(const FItemData& NewItem);

	UFUNCTION(BlueprintCallable, Category = "Player")
	int32 GetGold() const { return Gold; }

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 PlayerLevel = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 CurrentExp = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 ExpToNextLevel = 4;

	UFUNCTION(BlueprintCallable)
	void AddExp(int32 Amout);

};
