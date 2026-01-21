// DuckUnit.cpp

#include "DuckUnit.h"
#include "Animation/AnimInstance.h"

ADuckUnit::ADuckUnit()
{
    MaxHP = 95.f;
    HP = MaxHP;

    Attack = 8.f;          // 初撃に使うならここは低めでOK
    Defense = 3.f;
    MagicDefense = 4.f;
    MagicPower = 0.f;      // 今は未使用でもOK

    Range = 150.f;         // 近距離
    MoveSpeed = 130.f;

    AttackInterval = 1.1f;
    BaseAttackInterval = 1.1f;

    Team = EUnitTeam::Player;
    UnitID = FName("Duck");

    CritChance = 0.0f;     // DoT役ならクリ無しでも良い
    CritMultiplier = 1.5f;

    // 毒調整（ここは好み）
    PoisonDuration = 4.0f;
    PoisonDamagePerTick = 3.0f;
    PoisonTickInterval = 1.0f;
    InitialHitDamage = 6.0f; // 0にすると「DoT専用」になる
}

void ADuckUnit::BeginPlay()
{
    Super::BeginPlay();
}

bool ADuckUnit::CanUseSkill() const
{
    return false;
}

void ADuckUnit::UseSkill(AUnit* Target)
{
}

void ADuckUnit::AttackTarget(AUnit* Target)
{
    UE_LOG(LogTemp, Warning, TEXT("[Duck] AttackTarget ENTER Target=%s Dead=%d"),
        Target ? *Target->GetName() : TEXT("NULL"),
        Target ? (Target->bIsDead ? 1 : 0) : -1);


    if (!Target || Target->bIsDead) return;

    bIsAttacking = true;
    PendingTarget = Target;

    // ① まずは確実に当てる（動作確認用）
    ApplyDuckHit(Target);

    // ② アニメはあとから（今は再生できたらラッキーくらい）
    if (UnitMesh)
    {
        UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance();
        UE_LOG(LogTemp, Warning, TEXT("[Duck] AnimInstance=%s Montage=%s"),
            AnimInstance ? *AnimInstance->GetName() : TEXT("NULL"),
            AttackMontage ? *AttackMontage->GetName() : TEXT("NULL"));

        if (AnimInstance && AttackMontage)
        {
            const float Len = AnimInstance->Montage_Play(AttackMontage, 1.0f);
            UE_LOG(LogTemp, Warning, TEXT("[Duck] Montage_Play Len=%.3f"), Len);
        }
    }

}

void ADuckUnit::ApplyDuckHit(AUnit* Target)
{
    UE_LOG(LogTemp, Warning, TEXT("[Duck] ApplyDuckHit ENTER Target=%s"), *Target->GetName());

    if (!Target || Target->bIsDead) return;

    // ① 初撃（任意）
    if (InitialHitDamage > 0.f)
    {
        // 毒っぽいので魔法ダメ扱い推奨
        // TakeMagicDamage が無いなら TakePhysicalDamage に置き換え
        Target->TakeMagicDamage(InitialHitDamage);
    }

    // ② 毒DoT付与（ターゲット側にタイマーが乗る）
    Target->ApplyPoison(PoisonDuration, PoisonDamagePerTick, PoisonTickInterval);

    UE_LOG(LogTemp, Warning,
        TEXT("Duck hit %s Initial=%.1f Poison: Dur=%.1f Tick=%.1f DPT=%.1f"),
        *Target->GetName(),
        InitialHitDamage,
        PoisonDuration,
        PoisonTickInterval,
        PoisonDamagePerTick
    );
}

void ADuckUnit::HandleMeleeHitNotify()
{
    UE_LOG(LogTemp, Warning, TEXT("[Duck] HandleMeleeHitNotify fired"));
    if (!PendingTarget.IsValid() || PendingTarget->bIsDead) return;
    ApplyDuckHit(PendingTarget.Get());
}
