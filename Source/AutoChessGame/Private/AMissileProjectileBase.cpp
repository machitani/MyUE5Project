#include "AMissileProjectileBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AAMissileProjectileBase::AAMissileProjectileBase()
{
    PrimaryActorTick.bCanEverTick = true;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->InitSphereRadius(18.f);

    // Arrowと同じ：当たり判定は“距離判定”でやるので衝突は無視
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    InitialLifeSpan = 5.f;
}

void AAMissileProjectileBase::BeginPlay()
{
    Super::BeginPlay();

    // 念のため：見える状態に
    if (Mesh)
    {
        Mesh->SetHiddenInGame(false);
        Mesh->SetVisibility(true);
    }
}

void AAMissileProjectileBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetUnit || !IsValid(TargetUnit) || TargetUnit->bIsDead || TargetUnit->HP <= 0.f)
    {
        Destroy();
        return;
    }

    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = TargetUnit->GetActorLocation();

    // 2DっぽくしたいならZ固定（必要なら）
    // TargetLoc.Z = MyLoc.Z;

    // ★追跡移動（毎Tickで狙う＝確実に追尾する）
    FVector Dir = (TargetLoc - MyLoc).GetSafeNormal();
    FVector NewLoc = MyLoc + Dir * Speed * DeltaTime;
    SetActorLocation(NewLoc);

    // ★命中判定（Arrowと同じ）
    const float DistSq = FVector::DistSquared(NewLoc, TargetLoc);
    const float HitRadiusSq = HitRadius * HitRadius;

    if (DistSq <= HitRadiusSq)
    {
        if (TargetUnit->Team != OwnerTeam && DamageAmount > 0.f)
        {
            TargetUnit->TakePhysicalDamage(DamageAmount);
        }
        Destroy();
    }
}
