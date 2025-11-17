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

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<FItemData> OwnedItems;

	UFUNCTION(BlueprintCallable)
	void AddItem(const FItemData& NewItem);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 PlayerLevel = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 CurrentExp = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 ExpToNextLevel = 4;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Level")
	int32 MaxUnitCount = 3;

	UFUNCTION(BlueprintCallable,Category="Level")
	void AddExp(int32 Amout);

	



};
