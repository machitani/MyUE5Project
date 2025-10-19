// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"

UCLASS()
class AUTOCHESSGAME_API AUnit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	///駒の見た目
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UStaticMeshComponent* UnitMesh;

	///ステータス
	UPROPERTY(EditAnywhere,BlueprintReadWrite,category="Stats")
	int32 HP = 10;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,CateGory="Stats")
	int32 Attack = 3;

};
