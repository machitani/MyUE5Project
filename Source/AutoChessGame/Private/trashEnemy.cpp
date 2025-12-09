// trashEnemy.cpp

#include "trashEnemy.h"
#include "Animation/AnimInstance.h"

ATrashEnemy::ATrashEnemy()
{
    // ★ ザコ敵っぽいステータス
    MaxHP = 60.f;
    HP = MaxHP;

    Attack = 10.f;  // そこそこ殴ってくる
    Defense = 1.f;
    MagicDefense = 1.f;
    MagicPower = 0.f;

    Range = 150.f; // 完全近接
    MoveSpeed = 130.f;
    AttackInterval = 1.0f;

    Team = EUnitTeam::Enemy;
    UnitID = FName("TrashEnemy");
}

void ATrashEnemy::BeginPlay()
{
    Super::BeginPlay();
}

bool ATrashEnemy::CanUseSkill() const
{
    // 親の Skill システムは使わない
    return false;
}

void ATrashEnemy::UseSkill(AUnit* Target)
{
    // 今のところ未使用（あとで何か入れたくなったらここへ）
}

void ATrashEnemy::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    // 攻撃中フラグON（AnimBPでアタックモーション再生に使う）
    bIsAttacking = true;

    // この攻撃で狙っている敵を保存（Notifyから使う）
    PendingTarget = Target;
}

void ATrashEnemy::ApplyMeleeDamage(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    // 普通に物理ダメージ
    Target->TakePhysicalDamage(Attack);
}

void ATrashEnemy::HandleMeleeHitNotify()
{
    // 攻撃中にターゲットが死んでた / 消えてたら何もしない
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead)
    {
        return;
    }

    // ★ ここが「当たりフレーム」で呼ばれる
    ApplyMeleeDamage(PendingTarget);
}
