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

    FVector Center = Target->GetActorLocation();

    // 周囲のプレイヤーユニットに範囲ダメージ
    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Other = *It;
        if (!Other) continue;
        if (Other->Team == Team) continue;         // 敵チーム（プレイヤー）だけ
        if (Other->bIsDead || Other->HP <= 0.f) continue;

        float Dist = FVector::Dist(Center, Other->GetActorLocation());
        if (Dist > MissileRadius) continue;

        float Damage = Attack * MissileDamageMultiplier;
        Other->TakePhysicalDamage(Damage);
    }

    UE_LOG(LogTemp, Warning,
        TEXT("[BossEnemy] Missile Barrage used at %s"),
        *Center.ToString());

    // クールタイムリセット
    SkillTimer = 0.f;
}

void ABossEnemy::AttackTarget(AUnit* Target)
{
    Super::AttackTarget(Target);
}
