#include "SuportEnemy.h"
#include "EngineUtils.h"
#include "Animation/AnimInstance.h"

ASuportEnemy::ASuportEnemy()
{
    MaxHP = 120.f;
    HP = MaxHP;

    Attack = 0.f;          // ★ サポ専なら殴らない（殴るなら値入れてOK）
    Defense = 3.f;
    MagicDefense = 3.f;
    MagicPower = 10.f;

    Range = HealRadius;    // ★ 射程 = 回復射程（好みで）
    MoveSpeed = 130.f;
    AttackInterval = 1.2f;

    Team = EUnitTeam::Enemy;
    UnitID = FName("SupportEnemy");

    bHasSkill = false;     // ★ 親の UseSkill ルートは使わない
    SkillCooldown = 0.f;
    SkillTimer = 0.f;

    // 向き
    bFaceTarget = true;
    FacingYawOffset = 0.f;
    RotationInterpSpeed = 10.f;
}

void ASuportEnemy::BeginPlay()
{
    Super::BeginPlay();
}

// ★ 味方（敵チーム内）から「一番HP割合が低い」やつを選ぶ
AUnit* ASuportEnemy::ChooseTarget() const
{
    AUnit* Best = nullptr;
    float LowestRatio = 1.1f;

    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Ally = *It;
        if (!Ally) continue;
        if (Ally == this) continue;
        if (Ally->Team != Team) continue;
        if (Ally->bIsDead || Ally->HP <= 0.f) continue;
        if (Ally->MaxHP <= 0.f) continue;

        // 回復範囲内だけ（※Targetまで寄って回復したいなら、この距離制限は外してもOK）
        const float Dist = FVector::Dist(GetActorLocation(), Ally->GetActorLocation());
        if (Dist > HealRadius) continue;

        const float Ratio = Ally->HP / Ally->MaxHP;
        if (Ratio >= 1.0f) continue; // 満タンは除外

        if (Ratio < LowestRatio)
        {
            LowestRatio = Ratio;
            Best = Ally;
        }
    }

    return Best;
}

void ASuportEnemy::AttackTarget(AUnit* Target)
{
    if (bIsDead) return;

    bIsAttacking = true;

    // Target は味方の想定
    PendingHealTarget = Target;

    // 念のため保険
    if (!PendingHealTarget || PendingHealTarget->Team != Team ||
        PendingHealTarget->bIsDead || PendingHealTarget->HP <= 0.f)
    {
        PendingHealTarget = ChooseTarget();
    }

    if (!PendingHealTarget) return;

    // モンタージュ再生（連打防止）
    if (UnitMesh)
    {
        if (UAnimInstance* Anim = UnitMesh->GetAnimInstance())
        {
            
        }
    }

    // ※ 実回復は Notify -> HandleHealNotify() で行う
}

void ASuportEnemy::HandleHealNotify()
{
    if (!PendingHealTarget || !IsValid(PendingHealTarget) ||
        PendingHealTarget->bIsDead || PendingHealTarget->HP <= 0.f)
    {
        PendingHealTarget = ChooseTarget();
    }
    if (!PendingHealTarget) return;

    // 射程チェック（回復射程）
    const float Dist = FVector::Dist(GetActorLocation(), PendingHealTarget->GetActorLocation());
    if (Dist > HealRadius) return;

    ApplyHeal(PendingHealTarget);
}

void ASuportEnemy::ApplyHeal(AUnit* Ally)
{
    if (!Ally || Ally->bIsDead) return;

    const float OldHP = Ally->HP;
    const float NewHP = FMath::Clamp(OldHP + HealAmount, 0.f, Ally->MaxHP);
    const float Actual = NewHP - OldHP;
    if (Actual <= 0.f) return;

    Ally->HP = NewHP;

    UE_LOG(LogTemp, Warning,
        TEXT("[SupportEnemy] Healed ally %s (+%.1f) HP=%.1f/%.1f"),
        *Ally->GetName(), Actual, Ally->HP, Ally->MaxHP);

    // もし回復ポップアップを敵側にも出したいなら（関数がpublicならOK）
    Ally->ShowHealPopup(Actual);
}
