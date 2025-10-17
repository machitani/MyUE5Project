// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardManager.h"
#include"Engine/World.h"

// Sets default values
ABoardManager::ABoardManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABoardManager::BeginPlay()
{
	Super::BeginPlay();
	GenerateBoard();
}

// Called every frame
void ABoardManager::GenerateBoard()
{
	if (!TileClass)return;

	for (int32 X = 0; X < Columns; X++)
	{
		for (int32 Y = 0; Y < Rows; Y++)
		{
			FVector SpawnLocation = FVector(X * TileSpacing, Y * TileSpacing, 0.f);
			FActorSpawnParameters SpawnParams;
			ATile* NewTile = GetWorld()->SpawnActor<ATile>(TileClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			if (NewTile)
			{
				NewTile->X = X;
				NewTile->Y = Y;
				Tiles.Add(NewTile);
			}
		}
	}

}

