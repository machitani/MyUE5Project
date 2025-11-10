// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "Blueprint/UserWidget.h"
#include "ItemBenchSlot.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API UItemBenchSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	//このスロットに入ってるアイテム
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
	FItemData ItemData;
	
};
