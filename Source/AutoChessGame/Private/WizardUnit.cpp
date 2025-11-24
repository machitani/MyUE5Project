#include "WizardUnit.h"

AWizardUnit::AWizardUnit()
{
    MaxHP = 90.f;
    HP = MaxHP;

    Attack = 8.f;     // 物理は弱め
    Defense = 1.f;
    MagicDefense = 4.f;
    MagicPower = 28.f;    // 魔力高め

    Range = 320.f;
    MoveSpeed = 130.f;
    AttackInterval = 1.3f;   // 少し遅め

    Team = EUnitTeam::Player;
    UnitID = FName("Wizard");
}

void AWizardUnit::BeginPlay()
{
    Super::BeginPlay();
}

bool AWizardUnit::CanUseSkill() const
{
    // ひとまず毎回スキル発動してOKにしておく
    return true;
}

void AWizardUnit::UseSkill(AUnit* Target)
{
    if (!Target) return;

    // シンプルな単体魔法ダメ：魔力 × 1.5
    const float MagicDamage = MagicPower * 1.5f;

    UE_LOG(LogTemp, Warning,
        TEXT("Wizard %s casts Spell on %s (%.1f magic damage)"),
        *GetName(), *Target->GetName(), MagicDamage);

    Target->TakeMagicDamage(MagicDamage);
}

void AWizardUnit::AttackTarget(AUnit* Target)
{
    if (!Target) return;

    // 基本攻撃（物理）はちょっとおまけ
    Super::AttackTarget(Target);
    // Super 側で CanUseSkill→UseSkill も呼ばれるので、
    // さらに何か追加したければここに書く
}
