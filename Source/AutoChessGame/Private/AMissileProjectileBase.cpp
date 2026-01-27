#include "AMissileProjectileBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EngineUtils.h"

AAMissileProjectileBase::AAMissileProjectileBase()
{
    PrimaryActorTick.bCanEverTick = true;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->InitSphereRadius(18.f);
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionObjectType(ECC_WorldDynamic);
    Collision->SetCollisionResponseToAllChannels(ECR_Block);
    Collision->SetNotifyRigidBodyCollision(true);
    Collision->OnComponentHit.AddDynamic(this, &AAMissileProjectileBase::OnHit);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.f;

    OwnerTeam = EUnitTeam::Enemy;
}

void AAMissileProjectileBase::BeginPlay()
{
    Super::BeginPlay();
    FVector Dir = (TargetLocation - GetActorLocation());
    Dir.Z = 0.f; // 2Dっぽくしたいなら。3Dで撃ちたいなら消してOK
    Dir = Dir.GetSafeNormal();
    ProjectileMovement->Velocity = Dir * Speed;
}

void AAMissileProjectileBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 「目標地点に十分近づいたら爆発」（床に当たらない環境でも動く）
    const float Dist = FVector::Dist2D(GetActorLocation(), TargetLocation);
    if (!bExploded && Dist < 35.f)
    {
        Explode(TargetLocation);
    }
}

void AAMissileProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bExploded) return;
    Explode(Hit.ImpactPoint);
}

void AAMissileProjectileBase::Explode(const FVector& Center)
{
    if (bExploded) return;
    bExploded = true;

    // ★ 今のUseSkill()と同じ：中心から半径内の「敵チーム（Ownerと違う）」全員にダメ
    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Other = *It;
        if (!Other) continue;
        if (Other->bIsDead || Other->HP <= 0.f) continue;
        if (Other->Team == OwnerTeam) continue; // 同チームは除外

        const float Dist = FVector::Dist(Center, Other->GetActorLocation());
        if (Dist > ExplosionRadius) continue;

        Other->TakePhysicalDamage(DamageAmount);
    }

    Destroy();
}
