// DuckUnit.cpp

#include "EngineUtils.h"
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
   
    if (!Target || Target->bIsDead) return;

    
    
    bIsAttacking = true;

    PendingTarget = Target;

    //// ① まずは確実に当てる（動作確認用）
    //ApplyDuckHit(Target);

    //// ② アニメはあとから（今は再生できたらラッキーくらい）
    //if (UnitMesh)
    //{
    //    UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance();
    //    UE_LOG(LogTemp, Warning, TEXT("[Duck] AnimInstance=%s Montage=%s"),
    //        AnimInstance ? *AnimInstance->GetName() : TEXT("NULL"),
    //        AttackMontage ? *AttackMontage->GetName() : TEXT("NULL"));

    //    if (AnimInstance && AttackMontage)
    //    {
    //        const float Len = AnimInstance->Montage_Play(AttackMontage, 1.0f);
    //        UE_LOG(LogTemp, Warning, TEXT("[Duck] Montage_Play Len=%.3f"), Len);
    //    }
    //}

}

void ADuckUnit::CheckForTarget(float DeltaTime)
{

    if (CurrentTarget && CurrentTarget->bIsPoisoned)
    {
        CurrentTarget = nullptr;
    }
    if (bIsDead) return;

    // 既存ターゲットが死んだら解除
    if (CurrentTarget)
    {
        if (CurrentTarget->bIsDead || CurrentTarget->HP <= 0.f)
        {
            CurrentTarget = nullptr;
            bIsAttacking = false;
        }
    }

    // ターゲットがいなければ探す（★毒なし優先）
    if (!CurrentTarget)
    {
        AUnit* ClosestNonPoison = nullptr;
        float ClosestNonPoisonDist = FLT_MAX;

        AUnit* ClosestAny = nullptr;
        float ClosestAnyDist = FLT_MAX;

        for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
        {
            AUnit* Other = *It;
            if (Other == this) continue;
            if (Other->Team == Team) continue;
            if (Other->bIsDead || Other->HP <= 0.f) continue;

            const float Dist = FVector::Dist(GetActorLocation(), Other->GetActorLocation());

            // 保険：全体の最寄り
            if (Dist < ClosestAnyDist)
            {
                ClosestAnyDist = Dist;
                ClosestAny = Other;
            }

            // ★毒じゃない敵を優先
            if (!Other->bIsPoisoned && Dist < ClosestNonPoisonDist)
            {
                ClosestNonPoisonDist = Dist;
                ClosestNonPoison = Other;
            }
        }

        CurrentTarget = ClosestNonPoison ? ClosestNonPoison : ClosestAny;
    }

    if (!CurrentTarget) return;

    // 射程内なら攻撃、射程外なら移動（AUnitと同じ）
    const float DistToTarget = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

    if (DistToTarget <= Range)
    {
        UpdateFacing(DeltaTime);

        TimeSinceLastAttack += DeltaTime;
        if (TimeSinceLastAttack >= AttackInterval)
        {
            TimeSinceLastAttack = 0.f;
            AttackTarget(CurrentTarget);
        }
    }
    else
    {
        bIsAttacking = false;

        FVector NewLoc = FMath::VInterpConstantTo(
            GetActorLocation(),
            CurrentTarget->GetActorLocation(),
            DeltaTime,
            MoveSpeed
        );
        NewLoc.Z = GetActorLocation().Z;
        SetActorLocation(NewLoc);
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

    bIsAttacking = false;
    CurrentTarget = nullptr;      // 次のTickで再探索させる
    PendingTarget.Reset();        // すでにWeakならこれ
    UE_LOG(LogTemp, Warning, TEXT("[Duck] After ApplyPoison: Target=%s Poisoned=%d"),
        *Target->GetName(), Target->bIsPoisoned ? 1 : 0);


}
void ADuckUnit::HandleMeleeHitNotify()
{
    UE_LOG(LogTemp, Warning, TEXT("[Duck] HandleMeleeHitNotify fired"));

    // ★ 追加：状態を全部ログに出す
    UE_LOG(LogTemp, Warning, TEXT("[Duck] PendingValid=%d Pending=%s  CurrentTarget=%s"),
        PendingTarget.IsValid() ? 1 : 0,
        PendingTarget.IsValid() ? *PendingTarget->GetName() : TEXT("NULL"),
        CurrentTarget ? *CurrentTarget->GetName() : TEXT("NULL"));

    if (!PendingTarget.IsValid() || PendingTarget->bIsDead) return;

    ApplyDuckHit(PendingTarget.Get());
}

