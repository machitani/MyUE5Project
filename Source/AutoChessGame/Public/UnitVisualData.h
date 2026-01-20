// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UnitVisualData.generated.h"

USTRUCT(BlueprintType)
struct FUnitVisualData:public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UTexture2D* Icon = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText RoleText; 
};