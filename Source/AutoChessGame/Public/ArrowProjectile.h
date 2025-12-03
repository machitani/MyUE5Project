// ArrowProjectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.h"
#include "ArrowProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class AUTOCHESSGAME_API AArrowProjectile : public AActor
{
    GENERATED_BODY()

public:
    AArrowProjectile();

    virtual void Tick(float DeltaTime) override;

    // ã|ï∫Ç©ÇÁê›íËÇ≥ÇÍÇÈèÓïÒ
    UPROPERTY(BlueprintReadWrite, Category = "Arrow")
    AUnit* TargetUnit = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Arrow")
    float DamageAmount = 0.f;            // Attack ÇÇªÇÃÇ‹Ç‹ìnÇ∑ÉCÉÅÅ[ÉW

    UPROPERTY(BlueprintReadWrite, Category = "Arrow")
    EUnitTeam OwnerTeam = EUnitTeam::Player;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow")
    float HitRadius = 40.f;

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* ArrowCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProjectileMovementComponent* ProjectileMovement;
};
