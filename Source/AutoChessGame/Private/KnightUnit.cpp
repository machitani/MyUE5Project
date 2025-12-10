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

    BaseAttackInterval = 1.0f;
    AttackInterval = 1.0f;

    Team = EUnitTeam::Player;
    UnitID = FName("Knight");

    CritChance = 0.15f; // 15% くらい（好みで調整）
    CritMultiplier = 1.5f; 
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

void AKnightUnit::ApplyMeleeDamage(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    bool bIsCrit = false;
    float DamageToApply = CalcPhysicalDamageWithCrit(Attack, bIsCrit);

    // ターゲット側に「今回のヒットはクリティカルだった」フラグを渡す
    Target->bLastHitWasCritical = bIsCrit;

    Target->TakePhysicalDamage(DamageToApply);

    UE_LOG(LogTemp, Warning,
        TEXT("Knight hit %s Damage=%.1f Crit=%d"),
        *Target->GetName(), DamageToApply, bIsCrit ? 1 : 0);
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

