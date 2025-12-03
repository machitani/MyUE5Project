#include "KnightUnit.h"
#include "Animation/AnimInstance.h"

AKnightUnit::AKnightUnit()
{
    MaxHP = 120.f;
    HP = MaxHP;

    Attack = 18.f;
    Defense = 6.f;
    MagicDefense = 3.f;
    MagicPower = 0.f;

    Range = 160.f;  // 近接らしい短めの射程
    MoveSpeed = 120.f;
    AttackInterval = 1.0f;  // ちょうど1秒とか好みで

    Team = EUnitTeam::Player;
    UnitID = FName("Knight");
}

void AKnightUnit::BeginPlay()
{
    Super::BeginPlay();
}

bool AKnightUnit::CanUseSkill() const
{
    return false; // とりあえずスキルはあとで
}

void AKnightUnit::UseSkill(AUnit* Target)
{
    // あとで「突き」や「盾バッシュ」追加したくなったらここに
}

void AKnightUnit::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    bIsAttacking = true;

    // この攻撃で狙っている敵を保存
    PendingTarget = Target;

    // すでに攻撃モーション中なら新しい攻撃を出さないようにしてもいい
    if (UnitMesh)
    {
        if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
        {
            if (AttackMontage)
            {
                // モンタージュ連打防止（任意）
                if (!AnimInstance->Montage_IsPlaying(AttackMontage))
                {
                    AnimInstance->Montage_Play(AttackMontage);
                }
            }
        }
    }

    // ★ 注意：
    // ここではもう Target->TakePhysicalDamage() は呼ばない！！
    // ダメージは HandleMeleeHitNotify() 内だけ。
}

void AKnightUnit::ApplyMeleeDamage(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    // 近接ダメージそのもの
    Target->TakePhysicalDamage(Attack);
}

void AKnightUnit::HandleMeleeHitNotify()
{
    // 攻撃中にターゲットが死んでた / いなくなってたらスキップ
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead)
    {
        return;
    }

    // ここが「剣が当たる瞬間」のダメージ発生地点
    ApplyMeleeDamage(PendingTarget);
}

