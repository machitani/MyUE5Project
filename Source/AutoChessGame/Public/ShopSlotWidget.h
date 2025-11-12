// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemData.h"
#include "ShopSlotWidget.generated.h"

/**
 * 
 */
class UImage;

UCLASS()
class AUTOCHESSGAME_API UShopSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
	FText ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
	int32 Price;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
	FName RowName;

	UPROPERTY(meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
	class AShopManager* ShopManagerRef;



	UFUNCTION(BlueprintCallable)
	void UpdateShopState();


	UFUNCTION(BlueprintCallable)
	void RefreshItemView(const FItemData& ItemData);

};
