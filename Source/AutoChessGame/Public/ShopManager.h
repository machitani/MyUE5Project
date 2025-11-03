// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopManager.generated.h"


// 仮のアイテムデータ
USTRUCT(BlueprintType)
struct FShopItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cost;

	// アイコンが後で必要なら
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
};

UCLASS()
class AUTOCHESSGAME_API AShopManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopManager();

	// 今ショップにあるアイテム（4個とか）
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FShopItemData> CurrentShopItems;

	// UIに更新命令を出す
	UFUNCTION(BlueprintCallable)
	void GenerateShopItems();
};
