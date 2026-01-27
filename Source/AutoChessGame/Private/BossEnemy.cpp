// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "EngineUtils.h"

ABossEnemy::ABossEnemy()
{
    MaxHP = 600.f;
    HP = MaxHP;

    Attack = 40.f;
    Defense = 10.f;
    MagicDefense = 8.f;
    MagicPower = 0.f;

    Range = 550.f;   // 長めの射程
    MoveSpeed = 120.f;   // ちょっと遅め
    AttackInterval = 1.5f;

    Team = EUnitTeam::Enemy;
    UnitID = FName("BossEnemy");

    // スキル（ミサイル範囲攻撃）
    bHasSkill = true;
    SkillCooldown = 6.0f;    // 6秒ごとにミサイル
    SkillTimer = 0.0f;

    MissileRadius = 600.f;
    MissileDamageMultiplier = 0.7f;

    // ターゲットの方向を向く
    bFaceTarget = true;
    FacingYawOffset = 0.f;
    RotationInterpSpeed = 10.f;
}

void ABossEnemy::BeginPlay()
{
    Super::BeginPlay();
}

bool ABossEnemy::CanUseSkill() const
{
    return bHasSkill && (SkillTimer >= SkillCooldown);
}

void ABossEnemy::UseSkill(AUnit* Target)
{
    if (!GetWorld() || !Target) return;
    if (!MissileClass) return;

    const FVector Center = Target->GetActorLocation();

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = GetInstigator();

    const FVector SpawnLoc = GetActorLocation() + MissileSpawnOffset;
    const FRotator SpawnRot = (Center - SpawnLoc).Rotation();

    AAMissileProjectileBase* M = GetWorld()->SpawnActor<AAMissileProjectileBase>(MissileClass, SpawnLoc, SpawnRot, Params);
    if (M)
    {
        M->OwnerTeam = Team; // Enemy
        M->TargetLocation = Center;
        M->ExplosionRadius = MissileRadius;
        M->DamageAmount = Attack * MissileDamageMultiplier;
    }

    // クールタイムリセット
    SkillTimer = 0.f;
}

void ABossEnemy::AttackTarget(AUnit* Target)
{
    Super::AttackTarget(Target);
}
