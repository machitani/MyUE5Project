#include "SniperEnemy.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyAssaultProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"


// もし弾クラスがあるなら include
// #include "EnemySniperProjectile.h"
// #include "GameFramework/ProjectileMovementComponent.h"

ASniperEnemy::ASniperEnemy()
{
    MaxHP = 180.f;
    HP = MaxHP;

    Attack = 55.f;
    Defense = 4.f;
    MagicDefense = 3.f;
    MagicPower = 0.f;

    Range = 900.f;
    MoveSpeed = 110.f;
    AttackInterval = 1.9f;

    Team = EUnitTeam::Enemy;
    UnitID = FName("SniperEnemy");
}

void ASniperEnemy::EndAttackLock()
{
    bIsAttacking = false;
}

void ASniperEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void ASniperEnemy::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    bIsAttacking = true;
    PendingTarget = Target;

    // ここでは「モンタージュ再生だけ」
    // ※ AUnitがACharacterじゃない場合は GetMesh() は使えない
    // AUnitが持ってるSkeletalMeshComponent（例: UnitMesh）を使う
    SpawnSniperBullet(Target);

    // ★最初は動作確認のため即発射にしたいなら、ここで呼んでもOK
    // SpawnSniperBullet(Target);
}

void ASniperEnemy::HandleSniperShootNotify()
{
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead) return;

    // 0なら即発射
    if (FireDelay <= 1.f)
    {
        SpawnSniperBullet(PendingTarget);
        return;
    }

    GetWorldTimerManager().SetTimer(
        FireDelayHandle,
        this,
        &ASniperEnemy::FireDelayed,
        FireDelay,
        false
    );
}

void ASniperEnemy::FireDelayed()
{
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead) return;
    SpawnSniperBullet(PendingTarget);
}

void ASniperEnemy::SpawnSniperBullet(AUnit* Target)
{
    if (!SniperBulletClass || !Target) return;
    UWorld* World = GetWorld();
    if (!World) return;

    const FVector SpawnLocation = GetActorLocation() + MuzzleOffset;

    FVector TargetLocation = Target->GetActorLocation();
    TargetLocation.Z = SpawnLocation.Z;

    FVector Dir = (TargetLocation - SpawnLocation);
    Dir.Z = 0.f;
    Dir.Normalize();

    const FRotator SpawnRotation = Dir.Rotation();

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AEnemyAssaultProjectile* P = World->SpawnActor<AEnemyAssaultProjectile>(
        SniperBulletClass, SpawnLocation, SpawnRotation, Params);

    UE_LOG(LogTemp, Warning, TEXT("[SniperEnemy] SpawnActor result=%s"), *GetNameSafe(P));
    if (!P) return;

    // ここが大事：弾に情報を入れて、速度を与える
    P->TargetUnit = Target;
    P->DamageAmount = Attack * BulletDamageMultiplier;
    P->OwnerTeam = Team;

    if (P->ProjectileMovement)
    {
        P->ProjectileMovement->Velocity = Dir * P->ProjectileMovement->InitialSpeed;
        P->ProjectileMovement->Activate(true);
    }
   
}
