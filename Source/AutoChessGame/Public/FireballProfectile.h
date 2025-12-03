// FireballProfectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.h"   // AUnit / EUnitTeam 用
#include "FireballProfectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class AUTOCHESSGAME_API AFireballProfectile : public AActor
{
    GENERATED_BODY()

public:
    AFireballProfectile();

    virtual void Tick(float DeltaTime) override;

    // Wizard がセットする情報
    UPROPERTY(BlueprintReadWrite, Category = "Fireball")
    AUnit* TargetUnit = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Fireball")
    float DamageAmount = 0.f;

    UPROPERTY(BlueprintReadWrite, Category = "Fireball")
    EUnitTeam OwnerTeam = EUnitTeam::Player;

    // どのくらい近づいたら「当たり」とみなすか
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fireball")
    float HitRadius = 40.f;

protected:
    virtual void BeginPlay() override;

public:
    // ★ コリジョンは FireballCollision という名前
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* FireballCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProjectileMovementComponent* ProjectileMovement;
};
