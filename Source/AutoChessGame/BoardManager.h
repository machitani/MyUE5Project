// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "BoardManager.generated.h"

UCLASS()
class AUTOCHESSGAME_API ABoardManager : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ABoardManager();

	virtual void BeginPlay()override;

/// <summary>
/// グリッドサイズ
/// </summary>
UPROPERTY(EditAnywhere)
int32 Rows = 7;
UPROPERTY(EditAnywhere)
int32 Columns = 7;

/// <summary>
/// タイルの間隔
/// </summary>
UPROPERTY(EditAnywhere)
float TileSpacing = 100.f;

/// <summary>
/// タイルの参照
/// </summary>
UPROPERTY(EditAnywhere)
TSubclassOf<ATile> TileClass;

/// <summary>
/// グリッド保存用
/// </summary>
TArray<ATile*>Tiles;

void GenerateBoard();

//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;

};
