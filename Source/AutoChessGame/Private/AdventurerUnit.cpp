#include "AdventurerUnit.h"

AAdventurerUnit::AAdventurerUnit()
{
    // 後衛DPSっぽいステータス
    MaxHP = 120.f;
    HP = MaxHP;

    Attack = 17.f;     // そこそこ高火力
    Defense = 3.f;
    MagicDefense = 2.f;
    MagicPower = 0.f;      // 物理寄り

    Range = 380.f;    // 前衛より長め、アーチャーよりちょい短めでもOK
    MoveSpeed = 150.f;
    AttackInterval = 1.0f;    // そこそこの連射速度

    Team = EUnitTeam::Player;
    UnitID = FName("Adventurer");
}

void AAdventurerUnit::BeginPlay()
{
    Super::BeginPlay();
}

void AAdventurerUnit::AttackTarget(AUnit* Target)
{
    if (!Target) return;

    // ターゲット方向を向かせる（見た目用）
    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
    ToTarget.Z = 0.f;
    FRotator NewRot = ToTarget.Rotation();
    SetActorRotation(NewRot);

    // ★ いまは即時ヒットの簡易版（パチンコの「当たった後」だけ表現）
    //    将来的にはここで Projectile を Spawn してもOK
    Super::AttackTarget(Target);

    // --- 将来的なイメージ ---
    // 1. パチンコ発射アニメーション再生（Montage 再生）
    // 2. 弾（Projectile）Spawn
    // 3. 弾が当たったタイミングでダメージ
}
