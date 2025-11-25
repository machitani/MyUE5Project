// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
class UTextBlock;
class UProgressBar;
class APlayerManager;

UCLASS()
class AUTOCHESSGAME_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct()override;
	virtual void NativeTick(const FGeometry& MyGeometyr, float DeltaTime)override;

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation
	) override;

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* LevelText;

	UPROPERTY(meta=(BindWidget))
	UProgressBar* ExpBar;

	UPROPERTY()
	APlayerManager* PlayerManagerRef;
};
