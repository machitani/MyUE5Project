// FireballProfectile.cpp

#include "FireballProfectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AFireballProfectile::AFireballProfectile()
{
    PrimaryActorTick.bCanEverTick = true; // 距離判定するので Tick を使う

    // ★ FireballCollision
    FireballCollision = CreateDefaultSubobject<USphereComponent>(TEXT("FireballCollision"));
    RootComponent = FireballCollision;
    FireballCollision->InitSphereRadius(15.f);

    // シンプルな QueryOnly の当たり判定（重力も物理も使わない）
    FireballCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    FireballCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    FireballCollision->SetSimulatePhysics(false);
    FireballCollision->SetEnableGravity(false);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetSimulatePhysics(false);
    Mesh->SetEnableGravity(false);

    ProjectileMovement =
        CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 600.f;
    ProjectileMovement->MaxSpeed = 600.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.f; // 落ちない

    InitialLifeSpan = 5.f; // 念のための自動消滅
}

void AFireballProfectile::BeginPlay()
{
    Super::BeginPlay();
}

void AFireballProfectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ターゲットが消えた / 死んだら自分も消す
    if (!TargetUnit || !IsValid(TargetUnit) || TargetUnit->bIsDead)
    {
        Destroy();
        return;
    }

    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = TargetUnit->GetActorLocation();

    // 上から視点なので高さを揃える
    TargetLoc.Z = MyLoc.Z;

    const float DistSq = FVector::DistSquared(MyLoc, TargetLoc);
    const float HitRadiusSq = HitRadius * HitRadius;

    if (DistSq <= HitRadiusSq)
    {
        if (TargetUnit->Team != OwnerTeam && DamageAmount > 0.f)
        {
            TargetUnit->TakeMagicDamage(DamageAmount);
        }

        Destroy();
    }
}
