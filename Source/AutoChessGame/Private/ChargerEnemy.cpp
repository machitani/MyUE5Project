// Fill out your copyright notice in the Description page of Project Settings.


#include "ChargerEnemy.h"

AChargerEnemy::AChargerEnemy()
{
    // 前衛アタッカー寄りのステータス（好みで調整してOK）
    MaxHP = 150.f;
    HP = MaxHP;

    Attack = 26.f;  // Knight より高火力
    Defense = 5.f;   // Knight より少し柔らかい
    MagicDefense = 2.f;
    MagicPower = 0.f;

    Range = 200.f; // 近接
    MoveSpeed = 180.f; // Knight より少し速め
    AttackInterval = 1.1f; // 同じくらい

    Team = EUnitTeam::Enemy;
    UnitID = FName("Charger");

    // 敵はドラッグできないようにしたい場合
    bCanDrag = false;
}

void AChargerEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void AChargerEnemy::AttackTarget(AUnit* Target)
{
    Super::AttackTarget(Target);
}
