#include "BlockKnightUnit.h"
#include "EngineUtils.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Engine/World.h"

ABlockKnightUnit::ABlockKnightUnit()
{
    // 例：前衛っぽいステータスに（好きに調整してOK）
    MaxHP = 110.f;
    HP = MaxHP;

    Attack = 12.f;
    Defense = 6.f;
    MagicDefense = 3.f;
    MagicPower = 0.f;

    MoveSpeed = 140.f;

    AttackInterval = 1.2f;
    BaseAttackInterval = 1.2f;

    Team = EUnitTeam::Player;
    UnitID = FName("BlockKnight");

    CritChance = 0.f;
    CritMultiplier = 1.f;
}

void ABlockKnightUnit::BeginPlay()
{
    Super::BeginPlay();

    // ★敵がいなくても支援できるように、タイマーで回す
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            SupportLoopHandle,
            this,
            &ABlockKnightUnit::TrySupport,
            SupportTick,
            true
        );
    }
}

// ★ 一番HPが低い味方を探す
AUnit* ABlockKnightUnit::FindLowestHpAlly() const
{
    AUnit* Lowest = nullptr;
    float LowestHpRatio = FLT_MAX;

    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Ally = *It;
        if (!Ally) continue;
        if (Ally == this) continue;
        if (Ally->Team != Team) continue;
        if (Ally->bIsDead || Ally->HP <= 0.f) continue;
        if (Ally->MaxHP <= 0.f) continue;

        // どっちでもOK：
        // ① HP実数が一番低い味方
        // float Score = Ally->HP;

        // ② HP割合が一番低い味方（おすすめ：タンクでも候補になる）
        const float Score = Ally->HP / Ally->MaxHP;

        if (Score < LowestHpRatio)
        {
            LowestHpRatio = Score;
            Lowest = Ally;
        }
    }

    return Lowest;
}

void ABlockKnightUnit::TrySupport()
{
    if (bIsDead) return;

    PendingSupportTarget = FindLowestHpAlly();
    if (!PendingSupportTarget) return;

    const float Dist = FVector::Dist(GetActorLocation(), PendingSupportTarget->GetActorLocation());
    if (Dist > SupportRange) return;

    // モンタージュ連打防止（Rabbitと同じ）
    if (UnitMesh)
    {
        if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
        {
            if (SupportMontage && !AnimInstance->Montage_IsPlaying(SupportMontage))
            {
                AnimInstance->Montage_Play(SupportMontage);
            }
        }
    }

    // 実際の付与は AnimNotify -> HandleSupportNotify() で実行
}

void ABlockKnightUnit::HandleSupportNotify()
{
    // Notifyが来たタイミングでターゲットが死んでたら再探索
    if (!PendingSupportTarget || !IsValid(PendingSupportTarget) ||
        PendingSupportTarget->bIsDead || PendingSupportTarget->HP <= 0.f)
    {
        PendingSupportTarget = FindLowestHpAlly();
    }

    if (!PendingSupportTarget) return;

    const float Dist = FVector::Dist(GetActorLocation(), PendingSupportTarget->GetActorLocation());
    if (Dist > SupportRange) return;

    ApplyDefenseBuff(PendingSupportTarget);
}

void ABlockKnightUnit::ApplyDefenseBuff(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    const TWeakObjectPtr<AUnit> Key(Target);

    // 初回だけ元のDefenseを保存
    if (!SavedDefense.Contains(Key))
    {
        SavedDefense.Add(Key, Target->Defense);
    }

    // SavedDefense基準で固定（重ね掛けで増えない・延長だけ）
    Target->Defense = SavedDefense[Key] * DefenseMultiplier;

    // タイマー張り直しで延長
    if (UWorld* World = GetWorld())
    {
        FTimerHandle& Handle = BuffTimerHandles.FindOrAdd(Key);
        World->GetTimerManager().ClearTimer(Handle);
        World->GetTimerManager().SetTimer(
            Handle,
            FTimerDelegate::CreateUObject(this, &ABlockKnightUnit::RemoveDefenseBuff, Target),
            BuffDuration,
            false
        );
    }

    UE_LOG(LogTemp, Warning, TEXT("BlockKnight Buff -> %s DEF x%.2f (%.1fs)"),
        *Target->GetName(), DefenseMultiplier, BuffDuration);

    Target->ShowBuffPopup(TEXT("DEF UP"));
}

void ABlockKnightUnit::RemoveDefenseBuff(AUnit* Target)
{
    if (!Target || !IsValid(Target)) return;

    const TWeakObjectPtr<AUnit> Key(Target);

    if (SavedDefense.Contains(Key))
    {
        Target->Defense = SavedDefense[Key];
        SavedDefense.Remove(Key);
    }

    if (BuffTimerHandles.Contains(Key))
    {
        BuffTimerHandles.Remove(Key);
    }

    UE_LOG(LogTemp, Warning, TEXT("BlockKnight Buff End -> %s"), *Target->GetName());
}
