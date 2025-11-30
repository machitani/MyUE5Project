// Fill out your copyright notice in the Description page of Project Settings.


#include "TankEnemy.h"

ATankEnemy::ATankEnemy()
{
    MaxHP = 260.f;
    HP = MaxHP;

    Attack = 14.f;   // ナイトより少し低め
    Defense = 12.f;   // 物理めっちゃ硬い
    MagicDefense = 6.f;    // 魔法もそこそこ
    MagicPower = 0.f;

    Range = 220.f;  // ほぼ近接
    MoveSpeed = 110.f;  // 重いので遅め
    AttackInterval = 1.3f; // 振りが重いイメージ

    Team = EUnitTeam::Enemy;
    UnitID = FName("TankEnemy");

    // 敵はターゲット方向を向いて戦う
    bFaceTarget = true;
    FacingYawOffset = 0.f;   // メッシュの正面が +X なら 0 でOK
    RotationInterpSpeed = 8.f;   // 重いので旋回もちょい遅め
}

void ATankEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void ATankEnemy::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    // いまは普通の物理攻撃だけ（将来ノックバックとか付けるならここ）
    Super::AttackTarget(Target);
}
