#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.h"
#include "AMissileProjectileBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class AUTOCHESSGAME_API AAMissileProjectileBase : public AActor
{
    GENERATED_BODY()

public:
    AAMissileProjectileBase();
    virtual void Tick(float DeltaTime) override;

    // BossÇ©ÇÁê›íËÇ≥ÇÍÇÈ
    UPROPERTY(BlueprintReadWrite, Category = "Missile")
    AUnit* TargetUnit = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Missile")
    float DamageAmount = 0.f;

    UPROPERTY(BlueprintReadWrite, Category = "Missile")
    EUnitTeam OwnerTeam = EUnitTeam::Enemy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
    float Speed = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
    float HitRadius = 60.f; // É~ÉTÉCÉãÇÕè≠ÇµëÂÇ´ÇﬂÇ≈Ç‡OK

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;
};
