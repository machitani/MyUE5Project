#include "NurseUnit.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Animation/AnimInstance.h"

ANurseUnit::ANurseUnit()
{
    // ステータス：そこそこ硬いヒーラー
    MaxHP = 110.f;
    HP = MaxHP;

    Attack = 6.f;      // 攻撃は弱め
    Defense = 3.f;
    MagicDefense = 6.f;
    MagicPower = 25.f;     // 回復量ベース

    Range = 300.f;    // 回復対象を探す射程
    MoveSpeed = 130.f;
    AttackInterval = 1.2f;

    Team = EUnitTeam::Player;
    UnitID = FName("Nurse");
}

void ANurseUnit::BeginPlay()
{
    Super::BeginPlay();
}

bool ANurseUnit::CanUseSkill() const
{
    // ★ 親からはもう Skill を呼ばせない
    return false;
}

void ANurseUnit::UseSkill(AUnit* Target)
{
    // 今は使わない。将来別のスキルを作りたくなったらここに
}

void ANurseUnit::AttackTarget(AUnit* Target)
{
    if (bIsDead) return;

    bIsAttacking = true;

    // ★ 一応おまけ物理攻撃を残したいならここで殴る
    if (Target && !Target->bIsDead)
    {
        Target->TakePhysicalDamage(Attack);
    }

    // ★ このヒールで回復する予定の味方を決める
    PendingHealTarget = FindLowestHpAlly();

    // 誰もダメージ受けてないならアニメだけでもOK
    if (UnitMesh)
    {
        if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
        {
            if (HealMontage)
            {
                if (!AnimInstance->Montage_IsPlaying(HealMontage))
                {
                    AnimInstance->Montage_Play(HealMontage);
                }
            }
        }
    }

    // ※ Super::AttackTarget(Target) は呼ばない！！
    // 親から UseSkill を呼ばせる設計はやめて、Notify から回復する。
}

AUnit* ANurseUnit::FindLowestHpAlly() const
{
    float LowestHPRatio = 1.0f;
    AUnit* LowestAlly = nullptr;

    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Ally = *It;
        if (!Ally) continue;
        if (Ally->Team != Team) continue;
        if (Ally->HP <= 0.f) continue;

        float Ratio = Ally->HP / FMath::Max(Ally->MaxHP, 1.0f);
        if (Ratio < LowestHPRatio)
        {
            LowestHPRatio = Ratio;
            LowestAlly = Ally;
        }
    }

    return LowestAlly;
}

void ANurseUnit::ApplyHeal(AUnit* Ally)
{
    if (!Ally) return;

    const float HealAmount = MagicPower * 1.3f;

    float OldHP = Ally->HP;
    Ally->HP = FMath::Clamp(Ally->HP + HealAmount, 0.f, Ally->MaxHP);

    UE_LOG(LogTemp, Warning,
        TEXT("Nurse %s heals %s (%.1f -> %.1f)"),
        *GetName(), *Ally->GetName(), OldHP, Ally->HP);
}

void ANurseUnit::HandleHealNotify()
{
    // もし PendingHealTarget が死んでたり消えてたら、もう一回探す
    if (!PendingHealTarget || !IsValid(PendingHealTarget) || PendingHealTarget->HP <= 0.f)
    {
        PendingHealTarget = FindLowestHpAlly();
    }

    if (!PendingHealTarget) return;

    ApplyHeal(PendingHealTarget);
}

