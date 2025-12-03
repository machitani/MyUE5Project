#include "BearUnit.h"
#include "Animation/AnimInstance.h"

ABearUnit::ABearUnit()
{
    // タンク寄りステータス
    MaxHP = 220.f;
    HP = MaxHP;

    Attack = 12.f;
    Defense = 10.f;     // 物理にかなり強い
    MagicDefense = 4.f;
    MagicPower = 0.f;

    Range = 200.f;
    MoveSpeed = 120.f;
    AttackInterval = 1.2f;

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

    // この攻撃で狙っている敵を保存
    PendingTarget = Target;

    // ★ ここではダメージを与えない。アニメ再生だけ
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

    // 物理ダメージ（タンクだから控えめでもOK）
    Target->TakePhysicalDamage(Attack);
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
