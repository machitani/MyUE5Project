#include "AssaultEnemy.h"
#include "EnemyAssaultProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Animation/AnimInstance.h"

AAssaultEnemy::AAssaultEnemy()
{
    MaxHP = 70.f;
    HP = MaxHP;

    Attack = 16.f;   // 物理火力
    Defense = 2.f;
    MagicDefense = 1.f;
    MagicPower = 0.f;

    Range = 450.f;  // プレイヤー遠距離よりちょい長くしても面白い
    MoveSpeed = 120.f;
    AttackInterval = 0.8f;  // 連射気味アサルト

    Team = EUnitTeam::Enemy;
    UnitID = FName("AssaultEnemy");
}

void AAssaultEnemy::BeginPlay()
{
    Super::BeginPlay();
}

bool AAssaultEnemy::CanUseSkill() const
{
    return false; // まずは通常攻撃のみ
}

void AAssaultEnemy::UseSkill(AUnit* Target)
{
    // 後でグレネードとかスプレー撃ちとかやりたくなったとき用
}

void AAssaultEnemy::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    bIsAttacking = true;
    PendingTarget = Target;

    // まずはシンプルに「攻撃タイミングで即弾を出す」
    SpawnAssaultBullet(Target);

    // 将来的にアニメ同期するときは：
    // - ここでモンタージュ再生だけして
    // - 実際の SpawnAssaultBullet は AnimNotify から呼ぶ形に差し替えればOK
}

void AAssaultEnemy::SpawnAssaultBullet(AUnit* Target)
{
    if (!AssaultBulletClass || !Target) return;

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

    AEnemyAssaultProjectile* P = World->SpawnActor<AEnemyAssaultProjectile>(
        AssaultBulletClass, SpawnLocation, SpawnRotation, Params);

    if (P && P->ProjectileMovement)
    {
        const float PhysicalDamage = Attack * BulletDamageMultiplier;

        P->TargetUnit = Target;
        P->DamageAmount = PhysicalDamage;
        P->OwnerTeam = Team;  // = Enemy

        P->ProjectileMovement->Velocity =
            Dir * P->ProjectileMovement->InitialSpeed;
    }
}

void AAssaultEnemy::HandleAssaultShootNotify()
{
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead)
    {
        return;
    }

    SpawnAssaultBullet(PendingTarget);
}