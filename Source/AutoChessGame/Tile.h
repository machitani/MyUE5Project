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

	///グリッド座標
	int32 X;
	int32 Y;

	///タイルを選択可能かどうか
	bool bIsOccupied;

	///マテリアルを変えて選択表示などもできる
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TileMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
