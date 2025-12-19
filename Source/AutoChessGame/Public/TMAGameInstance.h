// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TMAGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AUTOCHESSGAME_API UTMAGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stage")
	int32 SelectedStageIndex = 1;
};
