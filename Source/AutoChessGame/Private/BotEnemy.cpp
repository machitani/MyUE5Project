// Fill out your copyright notice in the Description page of Project Settings.


#include "BotEnemy.h"
#include "EnemyAssaultProjectile.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Animation/AnimInstance.h"

ABotEnemy::ABotEnemy()
{
    MaxHP = 180.f;
    HP = MaxHP;

    Attack = 18.f;
    Defense = 8.f;
    MagicDefense = 3.f;
    MagicPower = 0.f;

    Range = 200.f;   // 近接
    MoveSpeed = 140.f;
    AttackInterval = 1.1f;

    Team = EUnitTeam::Enemy;
    UnitID = FName("BotEnemy");

    // 敵もターゲットの方向を向かせる
    bFaceTarget = true;
    FacingYawOffset = 0.f;   // メッシュの向きに合わせて調整したくなったらここ
    RotationInterpSpeed = 10.f;  // 向き変えるスピード
}

void ABotEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void ABotEnemy::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    bIsAttacking = true;
    PendingTarget = Target;

    // ★ まずは「攻撃タイミングで即ビーム発射」にしておく
    SpawnBusterShot(Target);
}

bool ABotEnemy::CanUseSkill() const
{
    return false;
}

void ABotEnemy::UseSkill(AUnit* Target)
{
}

void ABotEnemy::SpawnBusterShot(AUnit* Target)
{
    if (!BusterBulletClass || !Target) return;

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
        BusterBulletClass, SpawnLocation, SpawnRotation, Params);

    if (P && P->ProjectileMovement)
    {
        const float PhysicalDamage = Attack * BusterDamageMultiplier;

        P->TargetUnit = Target;
        P->DamageAmount = PhysicalDamage;
        P->OwnerTeam = Team;  // Enemy

        P->ProjectileMovement->Velocity =
            Dir * P->ProjectileMovement->InitialSpeed;
    }
}

void ABotEnemy::HandleBusterShootNotify()
{
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead)
    {
        return;
    }

    SpawnBusterShot(PendingTarget);
}
