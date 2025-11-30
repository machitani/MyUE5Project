// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultEnemy.h"

AAssaultEnemy::AAssaultEnemy()
{
    // ▼ ステータス設定（中距離DPS寄り）
    MaxHP = 120.f;
    HP = MaxHP;

    Attack = 20.f;   // ナイトよりちょい高めでもOK
    Defense = 2.f;    // 近接よりは柔らかい
    MagicDefense = 2.f;
    MagicPower = 0.f;

    Range = 700.f;  // 中距離（ナイトよりかなり長い）
    MoveSpeed = 130.f;  // ちょい遅めでもOK
    AttackInterval = 0.6f;  // 連射感を出したいので速め

    Team = EUnitTeam::Enemy;
    UnitID = FName("Assault");  // WaveデータやSave用のID

    // 敵は常にターゲットの方向を向いて戦う想定
    bFaceTarget = true;
    FacingYawOffset = 0.f;   // メッシュの向きに合わせて必要なら微調整
    RotationInterpSpeed = 12.f; // 旋回速度
}

void AAssaultEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void AAssaultEnemy::AttackTarget(AUnit* Target)
{
    Super::AttackTarget(Target);
}
