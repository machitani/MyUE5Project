// NinjaUnit.cpp
#include "NinjaUnit.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ANinjaUnit::ANinjaUnit()
{
    MaxHP = 85.f;
    HP = MaxHP;

    Attack = 14.f;           // 単発はナイトより低め
    Defense = 3.f;
    MagicDefense = 3.f;
    MagicPower = 0.f;

    Range = 150.f;
    MoveSpeed = 150.f;

    AttackInterval = 0.85f;  // 少し速め
    BaseAttackInterval = AttackInterval;

    Team = EUnitTeam::Player;
    UnitID = FName("Ninja");

    CritChance = 0.25f;      // クリ高め
    CritMultiplier = 1.6f;

    SkillCooldown = 6.0f;
    SkillTimer = 0.0f;
    DashDistance = 450.0f;
    PostDashLockTime = 0.15f;
}

void ANinjaUnit::BeginPlay()
{
    Super::BeginPlay();
}

bool ANinjaUnit::CanUseSkill() const
{
    // スキルは「突進」：クールダウン管理
    return SkillTimer >= SkillCooldown;
}

void ANinjaUnit::UseSkill(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;
    if (!GetWorld()) return;

    // 突進：ターゲット方向に距離分だけワープ（まずは簡単に）
    const FVector From = GetActorLocation();
    const FVector To = Target->GetActorLocation();

    FVector Dir = (To - From);
    Dir.Z = 0.f;
    Dir = Dir.GetSafeNormal();

    const FVector NewLoc = From + Dir * DashDistance;

    // 地面に埋まるの防止：Zはそのまま（必要ならLineTraceで調整）
    SetActorLocation(FVector(NewLoc.X, NewLoc.Y, From.Z), true);

    // ターゲットの方を向く
    const FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
    SetActorRotation(FRotator(0.f, LookRot.Yaw, 0.f));

    // 強すぎ防止：少しだけ攻撃ロック
    AttackLockUntilTime = GetWorld()->GetTimeSeconds() + PostDashLockTime;

    SkillTimer = 0.0f;
}

void ANinjaUnit::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;
    if (!GetWorld()) return;

    // 突進直後の硬直（任意）
    if (GetWorld()->GetTimeSeconds() < AttackLockUntilTime)
        return;

    bIsAttacking = true;
    PendingTarget = Target;

    // Knightと同じ：アニメ再生は一旦無し（Notifyでダメージ）
    // モンタージュを使うなら、ここで Montage_Play してOK
}

void ANinjaUnit::ApplyMeleeDamage(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    bool  bIsCrit = false;
    float DamageToApply = CalcPhysicalDamageWithCrit(Attack, bIsCrit);

    Target->bLastHitWasCritical = bIsCrit;
    Target->TakePhysicalDamage(DamageToApply);

    UE_LOG(LogTemp, Warning,
        TEXT("Ninja hit %s Damage=%.1f Crit=%d"),
        *Target->GetName(), DamageToApply, bIsCrit ? 1 : 0);
}

void ANinjaUnit::HandleMeleeHitNotify()
{
    if (!PendingTarget.IsValid())
        return;

    AUnit* Target = PendingTarget.Get();
    if (!Target || Target->bIsDead)
        return;

    ApplyMeleeDamage(Target);
}

AUnit* ANinjaUnit::FindAssassinTarget() const
{
    if (!GetWorld()) return nullptr;

    AUnit* Best = nullptr;
    float BestScore = -FLT_MAX;

    // ざっくり：ワールド上の全Unitから敵を探す（あなたの既存方式に合わせて置き換えてOK）
    TArray<AActor*> AllUnits;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), AllUnits);

    for (AActor* A : AllUnits)
    {
        AUnit* U = Cast<AUnit>(A);
        if (!U || U->bIsDead) continue;
        if (U->Team == Team) continue; // 敵だけ

        const float Dist = FVector::Dist2D(GetActorLocation(), U->GetActorLocation());
        if (Dist > 2500.f) continue; // 遠すぎるのは無視（調整）

        // ★ ロール優先（あなたのプロジェクトにロールが無ければ、UnitIDやクラスで判定でもOK）
        // ここは「仮」：UnitIDでざっくり判定（あなたのIDに合わせて増やしてOK）
        float RoleScore = 0.f;
        const FString ID = U->UnitID.ToString();

        if (ID.Contains(TEXT("Healer")) || ID.Contains(TEXT("Nurse")))
            RoleScore += 100.f;
        else if (ID.Contains(TEXT("Archer")) || ID.Contains(TEXT("Sniper")) || ID.Contains(TEXT("Ranged")))
            RoleScore += 70.f;
        else if (ID.Contains(TEXT("Mage")) || ID.Contains(TEXT("Wizard")))
            RoleScore += 60.f;
        else if (ID.Contains(TEXT("Buffer")) || ID.Contains(TEXT("Support")))
            RoleScore += 40.f;
        else if (ID.Contains(TEXT("Tank")))
            RoleScore -= 30.f;

        // 距離：近いほど少し加点（近すぎ誘導を避けるため弱め）
        const float DistScore = FMath::Clamp(800.f - Dist, -800.f, 800.f) * 0.05f;

        const float Score = RoleScore + DistScore;

        if (Score > BestScore)
        {
            BestScore = Score;
            Best = U;
        }
    }

    return Best;
}
