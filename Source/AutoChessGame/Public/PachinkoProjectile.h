// PachinkoProjectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.h"
#include "PachinkoProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class AUTOCHESSGAME_API APachinkoProjectile : public AActor
{
    GENERATED_BODY()

public:
    APachinkoProjectile();

    virtual void Tick(float DeltaTime) override;

    // 冒険家から渡される
    UPROPERTY(BlueprintReadWrite, Category = "Pachinko")
    AUnit* TargetUnit = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Pachinko")
    float DamageAmount = 0.f;            // Attack ベース

    UPROPERTY(BlueprintReadWrite, Category = "Pachinko")
    EUnitTeam OwnerTeam = EUnitTeam::Player;

    // 当たり判定の許容距離
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pachinko")
    float HitRadius = 40.f;

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* PachinkoCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProjectileMovementComponent* ProjectileMovement;
};
