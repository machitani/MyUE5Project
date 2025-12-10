// PachinkoProjectile.cpp

#include "PachinkoProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Unit.h"
#include "GameFramework/ProjectileMovementComponent.h"

APachinkoProjectile::APachinkoProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    PachinkoCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PachinkoCollision"));
    RootComponent = PachinkoCollision;
    PachinkoCollision->InitSphereRadius(15.f);

    PachinkoCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PachinkoCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    PachinkoCollision->SetSimulatePhysics(false);
    PachinkoCollision->SetEnableGravity(false);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetSimulatePhysics(false);
    Mesh->SetEnableGravity(false);

    ProjectileMovement =
        CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 700.f;   // Wizardよりちょい速めとかお好みで
    ProjectileMovement->MaxSpeed = 700.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.f; // 落とさない

    InitialLifeSpan = 5.f;
}

void APachinkoProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void APachinkoProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetUnit || !IsValid(TargetUnit) || TargetUnit->bIsDead)
    {
        Destroy();
        return;
    }

    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = TargetUnit->GetActorLocation();

    // 上から視点なので高さをそろえる
    TargetLoc.Z = MyLoc.Z;

    const float DistSq = FVector::DistSquared(MyLoc, TargetLoc);
    const float HitRadiusSq = HitRadius * HitRadius;

    if (DistSq <= HitRadiusSq)
    {
        if (TargetUnit->Team != OwnerTeam && DamageAmount > 0.f)
        {
            AUnit* Attacker = Cast<AUnit>(GetOwner());

            float FinalDamage = DamageAmount;
            bool bIsCritical = false;

            if (Attacker)
            {
                FinalDamage = Attacker->CalcPhysicalDamageWithCrit(DamageAmount, bIsCritical);
            }

            TargetUnit->bLastHitWasCritical = bIsCritical;

            // パチンコは物理ダメージ扱い
            TargetUnit->TakePhysicalDamage(FinalDamage);
        }

        Destroy();
    }
}
