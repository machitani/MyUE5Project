// ArrowProjectile.cpp

#include "ArrowProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AArrowProjectile::AArrowProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    ArrowCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ArrowCollision"));
    RootComponent = ArrowCollision;
    ArrowCollision->InitSphereRadius(15.f);

    ArrowCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ArrowCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    ArrowCollision->SetSimulatePhysics(false);
    ArrowCollision->SetEnableGravity(false);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetSimulatePhysics(false);
    Mesh->SetEnableGravity(false);

    ProjectileMovement =
        CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 800.f;      // ‰Î‚Ì‹Ê‚æ‚è‚¿‚å‚¢‘‚­‚µ‚Ä‚àOK
    ProjectileMovement->MaxSpeed = 800.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.f;

    InitialLifeSpan = 5.f;
}

void AArrowProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void AArrowProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetUnit || !IsValid(TargetUnit) || TargetUnit->bIsDead)
    {
        Destroy();
        return;
    }

    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = TargetUnit->GetActorLocation();

    TargetLoc.Z = MyLoc.Z;

    const float DistSq = FVector::DistSquared(MyLoc, TargetLoc);
    const float HitRadiusSq = HitRadius * HitRadius;

    if (DistSq <= HitRadiusSq)
    {
        if (TargetUnit->Team != OwnerTeam && DamageAmount > 0.f)
        {
            // š •¨—ƒ_ƒ[ƒW
            TargetUnit->TakePhysicalDamage(DamageAmount);
        }

        Destroy();
    }
}
