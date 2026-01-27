// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "EngineUtils.h"

ABossEnemy::ABossEnemy()
{
    MaxHP = 600.f;
    HP = MaxHP;

    Attack = 40.f;
    Defense = 10.f;
    MagicDefense = 8.f;
    MagicPower = 0.f;

    Range = 550.f;   // 長めの射程
    MoveSpeed = 120.f;   // ちょっと遅め
    AttackInterval = 1.5f;

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

    // 範囲内対象を集める
    TArray<AUnit*> Targets;
    for (TActorIterator<AUnit> It(World); It; ++It)
    {
        AUnit* U = *It;
        if (!U) continue;
        if (U->bIsDead || U->HP <= 0.f) continue;
        if (U->Team == Team) continue;

        if (FVector::Dist(Center, U->GetActorLocation()) <= MissileRadius)
        {
            Targets.Add(U);
        }
    }
    if (Targets.Num() == 0) return;

    const float TotalDamage = Attack * MissileDamageMultiplier;
    const float PerDamage = TotalDamage / Targets.Num();

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = GetInstigator();
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // ★確実

    // ★確実にカメラに映りやすい高さで出す
    const FVector SpawnLoc = GetActorLocation() + FVector(0.f, 0.f, 250.f) + MissileSpawnOffset;

    for (AUnit* T : Targets)
    {
        if (!T || !IsValid(T)) continue;

        const FVector Aim = T->GetActorLocation();
        const FRotator SpawnRot = (Aim - SpawnLoc).Rotation();

        const FTransform SpawnTM(SpawnRot, SpawnLoc);

        // ★Deferred Spawn（BeginPlay前に値を入れられる）
        AAMissileProjectileBase* M =
            GetWorld()->SpawnActorDeferred<AAMissileProjectileBase>(
                MissileClass,
                SpawnTM,
                this,
                GetInstigator(),
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn
            );

        if (M)
        {
            M->OwnerTeam = Team;
            M->TargetUnit = T;          // ★ここが確実に反映される
            M->TargetLocation = Aim;
            M->DamageAmount = PerDamage;
            M->ExplosionRadius = 0.f;

            M->FinishSpawning(SpawnTM);
        }

        UE_LOG(LogTemp, Warning, TEXT("[Boss] Spawn Missile(Deferred) -> %s Target=%s"),
            M ? *M->GetName() : TEXT("NULL"),
            T ? *T->GetName() : TEXT("NULL"));
    }

    SkillTimer = 0.f;
    bIsAttacking = false;
    PendingMissileTarget = nullptr;
}

