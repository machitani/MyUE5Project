// Fill out your copyright notice in the Description page of Project Settings.


#include "TankEnemy.h"
#include "Animation/AnimInstance.h"

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

void ATankEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ATankEnemy::AttackTarget(AUnit* Target)
{
    if (bIsDead) return;

    bIsAttacking = true;

    // ★ タンクは攻撃しないのでダメージは与えない
    // Target->TakePhysicalDamage(...) は呼ばない

    // すでにガード中なら新しいガードを重ねないようにするのもアリ
    if (bIsGuardActive)
    {
        return;
    }

    // ガードモーション再生
    if (UnitMesh)
    {
        if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
        {
            if (GuardMontage)
            {
                if (!AnimInstance->Montage_IsPlaying(GuardMontage))
                {
                    AnimInstance->Montage_Play(GuardMontage);
                }
            }
        }
    }

}

bool ATankEnemy::CanUseSkill() const
{
    return false;
}

void ATankEnemy::UseSkill(AUnit* Target)
{
}

void ATankEnemy::ApplyGuardedDamage(float& DamageAmount) const
{
    if (bIsGuardActive)
    {
        DamageAmount *= GuardDamageReduceRate; // 例: 半分にする
    }
}

void ATankEnemy::HandleGuardStartNotify()
{
    bIsGuardActive = true;

    // タイマーで終わらせるなら
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(GuardEndHandle);
        World->GetTimerManager().SetTimer(
            GuardEndHandle,
            this,
            &ATankEnemy::HandleGuardEnd,
            GuardDuration,
            false
        );
    }
}

void ATankEnemy::HandleGuardEnd()
{
    bIsGuardActive = false;
}

void ATankEnemy::TakePhysicalDamage(float DamageAmount)
{
    ApplyGuardedDamage(DamageAmount);

    // 親の物理ダメージ処理
    Super::TakePhysicalDamage(DamageAmount);
}

void ATankEnemy::TakeMagicDamage(float DamageAmount)
{
    ApplyGuardedDamage(DamageAmount);

    // 親の魔法ダメージ処理
    Super::TakeMagicDamage(DamageAmount);
}
