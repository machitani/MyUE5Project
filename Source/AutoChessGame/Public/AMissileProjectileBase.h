// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "GameFramework/Actor.h"
#include "AMissileProjectileBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class AUTOCHESSGAME_API AAMissileProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:
    AAMissileProjectileBase();

    // Boss Ç™ Spawnå„Ç…ê›íËÇ∑ÇÈ
    UPROPERTY(BlueprintReadWrite, Category = "Missile")
    FVector TargetLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite, Category = "Missile")
    float DamageAmount = 0.f;

    UPROPERTY(BlueprintReadWrite, Category = "Missile")
    float ExplosionRadius = 600.f;

    UPROPERTY(BlueprintReadWrite, Category = "Missile")
    EUnitTeam OwnerTeam; // èâä˙âªÇÕcppÇ≈

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
    float Speed = 1200.f;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void Explode(const FVector& Center);

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* Collision = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProjectileMovementComponent* ProjectileMovement = nullptr;

private:
    bool bExploded = false;
};
