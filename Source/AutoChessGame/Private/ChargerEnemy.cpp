// Fill out your copyright notice in the Description page of Project Settings.


#include "ChargerEnemy.h"

AChargerEnemy::AChargerEnemy()
{
    // 前衛アタッカー寄りのステータス（好みで調整してOK）
    MaxHP = 95.f;
    HP = MaxHP;

    Attack = 22.f;  // Knight より高火力
    Defense = 3.f;   // Knight より少し柔らかい
    MagicDefense = 2.f;
    MagicPower = 0.f;

    Range = 170.f; // 近接
    MoveSpeed = 150.f; // Knight より少し速め
    AttackInterval = 1.6f; // 同じくらい

    CritChance = 0.04;
    CritMultiplier = 1.5;

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
    if (!Target || Target->bIsDead) return;

    bIsAttacking = true;

    // この攻撃で狙ってるプレイヤーユニットを保存
    PendingTarget = Target;

    // ★ ここではダメージを出さない！ モーションだけ
    if (UnitMesh)
    {
        if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
        {
            if (AttackMontage)
            {
                // 連打防止はお好みで
                if (!AnimInstance->Montage_IsPlaying(AttackMontage))
                {
                    AnimInstance->Montage_Play(AttackMontage);
                }
            }
        }
    }

}

bool AChargerEnemy::CanUseSkill() const
{
    return false;
}

void AChargerEnemy::UseSkill(AUnit* Target)
{
}

void AChargerEnemy::ApplyMeleeDamage(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    // プレイヤーユニットに物理ダメージ
    Target->TakePhysicalDamage(Attack);
}

void AChargerEnemy::HandleMeleeHitNotify()
{
    // 攻撃中にターゲットが死んでた / 消えてたら何もしない
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead)
    {
        return;
    }

    // 斧が当たるフレームで呼ばれる想定
    ApplyMeleeDamage(PendingTarget);
}