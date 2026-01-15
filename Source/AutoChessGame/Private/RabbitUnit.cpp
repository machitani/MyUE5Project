#include "RabbitUnit.h"
#include "EngineUtils.h"              // TActorIterator
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Engine/World.h"

ARabbitUnit::ARabbitUnit()
{
    MaxHP = 85.f;
    HP = MaxHP;

    Attack = 0.f;
    Defense = 3.f;
    MagicDefense = 4.f;
    MagicPower = 0.f;

    MoveSpeed = 140.f;

    AttackInterval = 1.3f;
    BaseAttackInterval = 1.3f;

    Range = SupportRange;

    Team = EUnitTeam::Player;
    UnitID = FName("Rabbit");

    CritChance = 0.f;
    CritMultiplier = 1.f;
}

void ARabbitUnit::BeginPlay()
{
    Super::BeginPlay();
}

// ★ 一番近い味方を探す（ナースの FindLowestHpAlly と同じ立ち位置）
AUnit* ARabbitUnit::FindClosestAlly() const
{
    AUnit* ClosestAlly = nullptr;
    float ClosestDist = FLT_MAX;

    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Ally = *It;
        if (!Ally) continue;
        if (Ally == this) continue;
        if (Ally->Team != Team) continue;
        if (Ally->bIsDead || Ally->HP <= 0.f) continue;

        const float Dist = FVector::Dist(GetActorLocation(), Ally->GetActorLocation());
        if (Dist < ClosestDist)
        {
            ClosestDist = Dist;
            ClosestAlly = Ally;
        }
    }

    return ClosestAlly;
}

void ARabbitUnit::AttackTarget(AUnit* Target)
{
    if (bIsDead) return;

    bIsAttacking = true;

    // ★ ナース式：引数Target（敵）は使わず、自分で支援対象を決める
    PendingSupportTarget = FindClosestAlly();

    // 味方がいなければ何もしない（アニメだけしたいならここを変える）
    if (!PendingSupportTarget) return;

    // 射程チェック（支援射程）
    const float Dist = FVector::Dist(GetActorLocation(), PendingSupportTarget->GetActorLocation());
    if (Dist > SupportRange) return;

    // ★ ナースと同じ：モンタージュ連打防止
    if (UnitMesh)
    {
        if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
        {
            if (SupportMontage)
            {
                if (!AnimInstance->Montage_IsPlaying(SupportMontage))
                {
                    AnimInstance->Montage_Play(SupportMontage);
                }
            }
        }
    }

    // ※ Super::AttackTarget(Target) は呼ばない（支援はNotifyで発動）
}

void ARabbitUnit::HandleSupportNotify()
{
    // Pending が死んでたり無効なら、ナースと同じく再探索
    if (!PendingSupportTarget || !IsValid(PendingSupportTarget) || PendingSupportTarget->bIsDead || PendingSupportTarget->HP <= 0.f)
    {
        PendingSupportTarget = FindClosestAlly();
    }

    if (!PendingSupportTarget) return;

    ApplyAttackBuff(PendingSupportTarget);
}

void ARabbitUnit::ApplyAttackBuff(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    const TWeakObjectPtr<AUnit> Key(Target);

    // 初回だけ元の攻撃力を保存
    if (!SavedAttack.Contains(Key))
    {
        SavedAttack.Add(Key, Target->Attack);
    }

    // SavedAttack基準で固定（重ね掛けで増えない、延長だけ）
    Target->Attack = SavedAttack[Key] * AttackMultiplier;

    // タイマーを張り直して延長
    if (UWorld* World = GetWorld())
    {
        FTimerHandle& Handle = BuffTimerHandles.FindOrAdd(Key);
        World->GetTimerManager().ClearTimer(Handle);
        World->GetTimerManager().SetTimer(
            Handle,
            FTimerDelegate::CreateUObject(this, &ARabbitUnit::RemoveAttackBuff, Target),
            BuffDuration,
            false
        );
    }

    UE_LOG(LogTemp, Warning, TEXT("Rabbit Buff -> %s ATK x%.2f (%.1fs)"),
        *Target->GetName(), AttackMultiplier, BuffDuration);

    Target->ShowBuffPopup(TEXT("ATK UP"));
}

void ARabbitUnit::RemoveAttackBuff(AUnit* Target)
{
    if (!Target || !IsValid(Target)) return;

    const TWeakObjectPtr<AUnit> Key(Target);

    if (SavedAttack.Contains(Key))
    {
        Target->Attack = SavedAttack[Key];
        SavedAttack.Remove(Key);
    }

    if (BuffTimerHandles.Contains(Key))
    {
        BuffTimerHandles.Remove(Key);
    }

    UE_LOG(LogTemp, Warning, TEXT("Rabbit Buff End -> %s"), *Target->GetName());
}
