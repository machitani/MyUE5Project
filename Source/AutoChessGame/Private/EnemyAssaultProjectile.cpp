// EnemyAssaultProjectile.cpp

#include "EnemyAssaultProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AEnemyAssaultProjectile::AEnemyAssaultProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    BulletCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BulletCollision"));
    RootComponent = BulletCollision;
    BulletCollision->InitSphereRadius(8.f);

    // 物理は使わず QueryOnly でOK
    BulletCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BulletCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    BulletCollision->SetSimulatePhysics(false);
    BulletCollision->SetEnableGravity(false);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetSimulatePhysics(false);
    Mesh->SetEnableGravity(false);

    ProjectileMovement =
        CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1200.f;   // アサルトらしく速め
    ProjectileMovement->MaxSpeed = 1200.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.f; // 落ちない

    InitialLifeSpan = 3.f; // 当たらなかった弾の自動消滅
}

void AEnemyAssaultProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyAssaultProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ターゲットがいなくなった / 死んだら弾も消す
    if (!TargetUnit || !IsValid(TargetUnit) || TargetUnit->bIsDead)
    {
        Destroy();
        return;
    }

    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = TargetUnit->GetActorLocation();

    // 上から視点なので高さを合わせる
    TargetLoc.Z = MyLoc.Z;

    const float DistSq = FVector::DistSquared(MyLoc, TargetLoc);
    const float HitRadiusSq = HitRadius * HitRadius;

    if (DistSq <= HitRadiusSq)
    {
        // 敵 -> プレイヤー陣営だけに当てたい
        if (TargetUnit->Team != OwnerTeam && DamageAmount > 0.f)
        {
            TargetUnit->TakePhysicalDamage(DamageAmount);
        }

        Destroy();
    }
}
