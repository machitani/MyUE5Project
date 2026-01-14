#include "RabbitUnit.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Engine/World.h"

ARabbitUnit::ARabbitUnit()
{
    // サポート寄りの例ステータス
    MaxHP = 85.f;
    HP = MaxHP;

    Attack = 0.f;      // 自分の攻撃は未定なら 0 でOK（今は使わない）
    Defense = 3.f;
    MagicDefense = 4.f;
    MagicPower = 0.f;

    MoveSpeed = 140.f;

    // 支援の発動間隔（AIが AttackInterval を参照してるなら流用できる）
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

void ARabbitUnit::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    // 射程チェック（支援射程）
    const float Dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Dist > SupportRange) return;

    bIsAttacking = true; // ABP用：支援中も「攻撃中」と同じ扱いでOK
    PendingSupportTarget = Target;

    if (UnitMesh)
    {
        if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
        {
            if (SupportMontage)
            {
                AnimInstance->Montage_Play(SupportMontage);
            }
        }
    }
}

void ARabbitUnit::HandleSupportNotify()
{
    if (!PendingSupportTarget || !IsValid(PendingSupportTarget) || PendingSupportTarget->bIsDead)
    {
        return;
    }

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

    // 既にバフ中なら「延長だけ」したい場合：
    // → Attackは既に上がってるはずなので、再計算で二重掛けにならないように
    //    いったん SavedAttack を基準に固定する
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
