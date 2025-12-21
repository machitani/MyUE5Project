// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitlePlayerController.generated.h"

class UTitleMenuWidget;
 
UCLASS()
class AUTOCHESSGAME_API ATitlePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
    virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> TitleWidgetClass;

	UPROPERTY()
	UUserWidget* TitleWidgetInstance = nullptr;


};
