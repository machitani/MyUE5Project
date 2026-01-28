// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "EngineUtils.h"

ABossEnemy::ABossEnemy()
{
    MaxHP = 520.f;
    HP = MaxHP;

    Attack = 30.f;
    Defense = 6.f;
    MagicDefense = 4.f;
    MagicPower = 0.f;

    Range = 550.f;   // 長めの射程
    MoveSpeed = 120.f;   // ちょっと遅め
    AttackInterval = 1.5f;

    CritChance = 0.08f;
    CritMultiplier = 1.6f;

    Team = EUnitTeam::Enemy;
    UnitID = FName("BossEnemy");

    // スキル（ミサイル範囲攻撃）
    bHasSkill = true;
    SkillCooldown = 6.0f;    // 6秒ごとにミサイル
    SkillTimer = 0.0f;

    MissileRadius = 600.f;
    MissileDamageMultiplier = 0.7f;

    // ターゲットの方向を向く
    bFaceTarget = true;
    FacingYawOffset = 0.f;
    RotationInterpSpeed = 10.f;
}

void ABossEnemy::BeginPlay()
{
    Super::BeginPlay();
}

bool ABossEnemy::CanUseSkill() const
{
    return bHasSkill && (SkillTimer >= SkillCooldown);
}

void ABossEnemy::UseSkill(AUnit* Target)
{
    if (bIsDead) return;
    if (!Target || Target->bIsDead || Target->HP <= 0.f) return;

    bIsAttacking = true;

    // 弾はNotifyで出すので、ここではターゲットだけ保存
    PendingMissileTarget = Target;

    // モンタージュ再生（連打防止）
    if (UnitMesh)
    {
        if (UAnimInstance* Anim = UnitMesh->GetAnimInstance())
        {
            if (MissileMontage && !Anim->Montage_IsPlaying(MissileMontage))
            {
                Anim->Montage_Play(MissileMontage);
            }
        }
    }
}

void ABossEnemy::AttackTarget(AUnit* Target)
{
    if (bIsDead) return;

    // スキル撃てるならスキル（=ミサイルモンタージュ）
    if (CanUseSkill())
    {
        UseSkill(Target);
        return;
    }

    // それ以外は通常攻撃（親に任せる / またはボス用通常攻撃モンタージュへ）
    Super::AttackTarget(Target);
}

void ABossEnemy::HandleMissileHitNotify()
{
    UE_LOG(LogTemp, Warning, TEXT("[Boss] HandleMissileHitNotify fired"));
    if (bIsDead) return;

    UWorld* World = GetWorld();
    if (!World || !MissileClass) return;

    // ターゲット保険
    if (!PendingMissileTarget || !IsValid(PendingMissileTarget) ||
        PendingMissileTarget->bIsDead || PendingMissileTarget->HP <= 0.f)
    {
        PendingMissileTarget = CurrentTarget;
    }
    if (!PendingMissileTarget) return;

    const FVector Center = PendingMissileTarget->GetActorLocation();

    // ① 範囲内の「相手チーム」を集める（Boss=Enemy なら Player側）
    TArray<AUnit*> Targets;
    Targets.Reserve(16);

    for (TActorIterator<AUnit> It(World); It; ++It)
    {
        AUnit* U = *It;
        if (!U) continue;
        if (U->bIsDead || U->HP <= 0.f) continue;
        if (U->Team == Team) continue; // 同チーム除外

        const float Dist = FVector::Dist(Center, U->GetActorLocation());
        if (Dist > MissileRadius) continue;

        Targets.Add(U);
    }

    if (Targets.Num() == 0) return;

    // ② 総量を人数で割る（総ダメージは同じ）
    const float TotalDamage = Attack * MissileDamageMultiplier;
    const float PerDamage = TotalDamage / Targets.Num();

    // ③ Spawn位置（まずは確実に見える位置）
    FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 200.f + FVector(0.f, 0.f, 120.f);
    SpawnLoc += MissileSpawnOffset;

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = GetInstigator();
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // ④ 1体につき1発（Arrow方式のミサイルがTargetUnitを追う）
    for (AUnit* T : Targets)
    {
        if (!IsValid(T)) continue;

        const FVector Aim = T->GetActorLocation();
        const FRotator SpawnRot = (Aim - SpawnLoc).Rotation();

        AAMissileProjectileBase* M =
            World->SpawnActor<AAMissileProjectileBase>(MissileClass, SpawnLoc, SpawnRot, Params);

        if (M)
        {
            M->OwnerTeam = Team;
            M->TargetUnit = T;
            M->DamageAmount = PerDamage;

            // 好みで調整
            // M->Speed = 900.f;
            // M->HitRadius = 80.f;
        }
    }

    // CT開始
    SkillTimer = 0.f;

    // 状態リセット（止まり対策）
    bIsAttacking = false;
    PendingMissileTarget = nullptr;
}
