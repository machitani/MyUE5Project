#include "NurseUnit.h"
#include "Engine/World.h"
#include "EngineUtils.h"

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
    // ひとまず毎回発動OK（あとでクールダウン制にしても良い）
    return true;
}

void ANurseUnit::UseSkill(AUnit* Target)
{
    if (!OwningBoardManager) return;

    // HP割合が一番低い味方を探して回復
    AUnit* LowestAlly = nullptr;
    float LowestHPRatio = 1.0f;

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

    if (!LowestAlly) return;

    const float HealAmount = MagicPower * 1.3f;

    float OldHP = LowestAlly->HP;
    LowestAlly->HP = FMath::Clamp(LowestAlly->HP + HealAmount, 0.f, LowestAlly->MaxHP);

    UE_LOG(LogTemp, Warning,
        TEXT("Nurse %s heals %s (%.1f -> %.1f)"),
        *GetName(), *LowestAlly->GetName(), OldHP, LowestAlly->HP);
}

void ANurseUnit::AttackTarget(AUnit* Target)
{
    if (!Target) return;

    // 基本攻撃（物理）はおまけ
    Super::AttackTarget(Target);
    // Super 側で CanUseSkill → UseSkill も呼ばれる設計なので、
    // ここで特別な処理を足したいなら追記する
}
