#include "AMissileProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

AAMissileProjectileBase::AAMissileProjectileBase()
{
    PrimaryActorTick.bCanEverTick = true;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->InitSphereRadius(18.f);
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Collision->OnComponentHit.AddDynamic(this, &AAMissileProjectileBase::OnHit);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // ===== ★とりあえず必ず見える強制設定 =====
    Mesh->SetHiddenInGame(false, true);
    Mesh->SetVisibility(true, true);
    Mesh->SetOwnerNoSee(false);
    Mesh->SetOnlyOwnerSee(false);
    Mesh->SetRenderInMainPass(true);
    Mesh->bRenderInDepthPass = true;
    Mesh->CastShadow = false;

    // ★ピボット/オフセット事故を潰す：中心に寄せる
    Mesh->SetRelativeLocation(FVector::ZeroVector);
    Mesh->SetRelativeRotation(FRotator::ZeroRotator);

    // ★カリング事故を潰す：Bounds拡張
    Mesh->SetBoundsScale(20.f);

    // ★BPのメッシュが何かおかしくても、まず球を表示して確認できるようにする
    // （後で消してOK）
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMesh.Succeeded())
    {
        Mesh->SetStaticMesh(SphereMesh.Object);
        Mesh->SetRelativeScale3D(FVector(0.25f)); // でかすぎたら下げる
    }
    // ======================================

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.f;

    ProjectileMovement->bIsHomingProjectile = true;
    ProjectileMovement->HomingAccelerationMagnitude = HomingAccel;

    OwnerTeam = EUnitTeam::Enemy;
}

void AAMissileProjectileBase::BeginPlay()
{
    Super::BeginPlay();

    // ★自分（Owner=Boss）に当たって即爆発するのを防ぐ
    if (Collision)
    {
        Collision->IgnoreActorWhenMoving(GetOwner(), true);
        Collision->IgnoreActorWhenMoving(this, true);
    }

    if (!ProjectileMovement) return;

    ProjectileMovement->HomingAccelerationMagnitude = HomingAccel;
    ProjectileMovement->InitialSpeed = Speed;
    ProjectileMovement->MaxSpeed = Speed;

    FVector Aim = TargetLocation;

    if (TargetUnit && IsValid(TargetUnit))
    {
        ProjectileMovement->bIsHomingProjectile = true;
        ProjectileMovement->HomingTargetComponent = TargetUnit->GetRootComponent();
        Aim = TargetUnit->GetActorLocation();
        TargetLocation = Aim;
    }
    else
    {
        ProjectileMovement->bIsHomingProjectile = false;
    }

    FVector Dir = (Aim - GetActorLocation());
    Dir.Z = 0.f;
    Dir = Dir.GetSafeNormal();

    ProjectileMovement->Velocity = Dir * Speed;

    UE_LOG(LogTemp, Warning, TEXT("[Missile] BeginPlay %s Speed=%.1f Homing=%d Target=%s"),
        *GetName(), Speed, ProjectileMovement->bIsHomingProjectile,
        TargetUnit ? *TargetUnit->GetName() : TEXT("None"));

    if (ProjectileMovement)
    {
        ProjectileMovement->StopMovementImmediately();
    }
    SetLifeSpan(2.0f);

    if (Mesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Missile] StaticMesh=%s"),
            Mesh->GetStaticMesh() ? *Mesh->GetStaticMesh()->GetName() : TEXT("NULL"));

        UE_LOG(LogTemp, Warning, TEXT("[Missile] Vis=%d Hidden=%d Scale=%s"),
            Mesh->IsVisible(),
            Mesh->bHiddenInGame,
            *Mesh->GetComponentScale().ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Missile] Mesh component is NULL"));
    }
}

void AAMissileProjectileBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ★デバッグで位置を見える化（確実に見える）
    //DrawDebugSphere(GetWorld(), GetActorLocation(), 20.f, 12, FColor::Red, false, 0.f, 0, 2.f);

    FVector Aim = TargetLocation;
    if (TargetUnit && IsValid(TargetUnit) && !TargetUnit->bIsDead && TargetUnit->HP > 0.f)
    {
        Aim = TargetUnit->GetActorLocation();
        TargetLocation = Aim;
    }

    // ★3D距離 + 閾値を大きめ（すり抜け防止）
    const float Dist = FVector::Dist(GetActorLocation(), Aim);
    if (!bExploded && Dist < 120.f)
    {
        Explode(Aim);
    }
    //DrawDebugSphere(GetWorld(), GetActorLocation(), 120.f, 16, FColor::Red, false, 0.f, 0, 6.f);
}

void AAMissileProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bExploded) return;

    // ★Ownerには反応しない（保険）
    if (OtherActor == GetOwner()) return;

    Explode(Hit.ImpactPoint);
}

void AAMissileProjectileBase::Explode(const FVector& Center)
{
    if (bExploded) return;
    bExploded = true;

    // ★単体ダメ（総量維持のため）
    if (TargetUnit && IsValid(TargetUnit) && !TargetUnit->bIsDead && TargetUnit->HP > 0.f)
    {
        if (TargetUnit->Team != OwnerTeam)
        {
            TargetUnit->TakePhysicalDamage(DamageAmount);
        }
    }

    SetLifeSpan(2.0f);
}
