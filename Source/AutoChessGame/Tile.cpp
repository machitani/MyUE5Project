// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	// ���b�V������
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	RootComponent = TileMesh;

	bIsOccupied = false;
}

void ATile::SetTileColor(const FLinearColor& NewColor)
{
	if (!TileMesh) return;

	// ���I�}�e���A�����쐬�i�C���f�b�N�X0�Ԃ̃}�e���A����Ώہj
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

// Called every frame�i���͖��g�p�j
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
