// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	// メッシュ生成
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	RootComponent = TileMesh;

	bIsOccupied = false;
}

void ATile::SetTileColor(const FLinearColor& NewColor)
{
	if (!TileMesh) return;

	// 動的マテリアルを作成（インデックス0番のマテリアルを対象）
	UMaterialInstanceDynamic* DynMat = TileMesh->CreateAndSetMaterialInstanceDynamic(0);
	if (DynMat)
	{
		DynMat->SetVectorParameterValue(FName("BaseColor"), NewColor);
	}
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame（今は未使用）
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
