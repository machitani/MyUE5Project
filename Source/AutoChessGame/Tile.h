// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UCLASS()
class AUTOCHESSGAME_API ATile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** タイルのメッシュ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	UStaticMeshComponent* TileMesh;

	/** このマスがユニットに占有されているかどうか */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	bool bIsOccupied;

	/** タイルの色を設定 */
	UFUNCTION(BlueprintCallable, Category = "Tile")
	void SetTileColor(const FLinearColor& NewColor);
};
