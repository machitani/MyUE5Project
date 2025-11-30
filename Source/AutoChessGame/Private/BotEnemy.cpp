// Fill out your copyright notice in the Description page of Project Settings.


#include "BotEnemy.h"

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
    Super::AttackTarget(Target);
}
