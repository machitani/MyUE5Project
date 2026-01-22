#include "BearUnit.h"
#include "Animation/AnimInstance.h"

ABearUnit::ABearUnit()
{
    // タンク寄りステータス
    MaxHP = 170.f;
    HP = MaxHP;

    Attack = 12.f;
    Defense = 5.f;     // 物理にかなり強い
    MagicDefense = 4.f;
    MagicPower = 0.f;

    Range = 160.f;
    MoveSpeed = 105.f;
    AttackInterval = 1.2f;

    CritChance = 0.02f;
    CritMultiplier = 1.45f;

    AttackInterval = 1.0f;
    BaseAttackInterval = 1.0f;

    Team = EUnitTeam::Player;
    UnitID = FName("Bear");
}

void ABearUnit::BeginPlay()
{
    Super::BeginPlay();
}

void ABearUnit::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    bIsAttacking = true;
    PendingTarget = Target;

    if (UnitMesh)
    {
        if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
        {
            if (AttackMontage)
            {
                float PlayRate = 1.0f;

                if (AttackInterval > 0.f && BaseAttackInterval > 0.f)
                {
                    PlayRate = BaseAttackInterval / AttackInterval;
                }

                if (!AnimInstance->Montage_IsPlaying(AttackMontage))
                {
                    AnimInstance->Montage_Play(AttackMontage, PlayRate);
                }
            }
        }
    }
}

bool ABearUnit::CanUseSkill() const
{
    return false;
}


void ABearUnit::UseSkill(AUnit* Target)
{
}

void ABearUnit::ApplyMeleeDamage(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    bool bIsCrit = false;
    float DamageToApply = CalcPhysicalDamageWithCrit(Attack, bIsCrit);

    Target -> bLastHitWasCritical = bIsCrit;
    Target->TakePhysicalDamage(DamageToApply);

}

void ABearUnit::HandleMeleeHitNotify()
{
    // ターゲットが既に死んでたら何もしない
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead)
    {
        return;
    }

    // 「熊パンチが当たったフレーム」で呼ばれる想定
    ApplyMeleeDamage(PendingTarget);
}
