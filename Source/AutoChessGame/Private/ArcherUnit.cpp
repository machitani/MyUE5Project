// ArcherUnit.cpp

#include "ArcherUnit.h"
#include "ArrowProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Animation/AnimInstance.h"

AArcherUnit::AArcherUnit()
{
    MaxHP = 80.f;
    HP = MaxHP;

    Attack = 18.f;    // 物理高め
    Defense = 2.f;
    MagicDefense = 1.f;
    MagicPower = 0.f;

    Range = 400.f;  // Wizard よりちょい長めでもいい
    MoveSpeed = 140.f;
    AttackInterval = 1.1f;
    BaseAttackInterval = 1.0f;

    Team = EUnitTeam::Player;
    UnitID = FName("Archer");

    CritChance = 0.25f;   // 25% くらい
    CritMultiplier = 1.5f;
}

void AArcherUnit::BeginPlay()
{
    Super::BeginPlay();
}

bool AArcherUnit::CanUseSkill() const
{
    return false; // まずは通常攻撃だけ
}

void AArcherUnit::UseSkill(AUnit* Target)
{
    // 後でスキルを作りたくなったらここに
}

// ArcherUnit.cpp

void AArcherUnit::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    const float MinArrowDistance = 150.f;

    UE_LOG(LogTemp, Warning,
        TEXT("[Archer] AttackTarget Dist=%.1f (MinArrowDistance=%.1f)"),
        Distance, MinArrowDistance);

    // 近距離：矢を飛ばさず即ダメージ
    if (Distance < MinArrowDistance)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Archer] Close attack: NO projectile"));

        bIsAttacking = true;
        bUseProjectileThisAttack = false; // ★ この攻撃は矢を使わない
        PendingTarget = nullptr;          // 念のためクリア

        if (UnitMesh)
        {
            if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
            {
                if (AttackMontage)
                {
                    float PlayRate = 1.0f;
                    if (AttackInterval > 0.f && BaseAttackInterval > 0.f)
                    {
                        PlayRate = BaseAttackInterval / AttackInterval;
                    }
                    AnimInstance->Montage_Play(AttackMontage, PlayRate);
                }
            }
        }

        float  BaseDamage = Attack * ArrowDamageMultiplier;
        bool   bIsCritical = false;
        float  FinalDamage = CalcPhysicalDamageWithCrit(BaseDamage, bIsCritical);

        Target->bLastHitWasCritical = bIsCritical;
        Target->TakePhysicalDamage(FinalDamage);

        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Archer] Far attack: USE projectile"));


    // --- 遠距離：矢を使う ---
    bIsAttacking = true;
    bUseProjectileThisAttack = true;   // ★ この攻撃は矢を使う
    PendingTarget = Target;

    if (UnitMesh)
    {
        if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
        {
            if (AttackMontage)
            {
                float PlayRate = 1.0f;

                if (AttackInterval > 0.f && BaseAttackInterval > 0.f)
                {
                    PlayRate = BaseAttackInterval / AttackInterval;
                }

                AnimInstance->Montage_Play(AttackMontage, PlayRate);
            }
        }
    }

    // 矢は Notify → HandleArrowShootNotify → SpawnArrow
}



void AArcherUnit::SpawnArrow(AUnit* Target)
{
    if (!ArrowClass || !Target) return;

    UE_LOG(LogTemp, Warning, TEXT("ARROW"));

    UWorld* World = GetWorld();
    if (!World) return;

    const FVector SpawnLocation = GetActorLocation() + ArrowSpawnOffset;

    FVector TargetLocation = Target->GetActorLocation();
    TargetLocation.Z = SpawnLocation.Z;

    FVector Dir = (TargetLocation - SpawnLocation);
    Dir.Z = 0.f;
    Dir.Normalize();

    const FRotator SpawnRotation = Dir.Rotation();

    FActorSpawnParameters Params;
    Params.Owner = this;

    AArrowProjectile* P = World->SpawnActor<AArrowProjectile>(
        ArrowClass, SpawnLocation, SpawnRotation, Params);

    if (P && P->ProjectileMovement)
    {
        const float PhysicalDamage = Attack * ArrowDamageMultiplier;

        P->TargetUnit = Target;
        P->DamageAmount = PhysicalDamage;
        P->OwnerTeam = Team;

        P->ProjectileMovement->Velocity =
            Dir * P->ProjectileMovement->InitialSpeed;
    }
}

void AArcherUnit::HandleArrowShootNotify()
{
    UE_LOG(LogTemp, Warning,
        TEXT("[Archer] Notify fired. Pending=%s UseProj=%d"),
        PendingTarget ? *PendingTarget->GetName() : TEXT("NULL"),
        bUseProjectileThisAttack ? 1 : 0);

    // ★ この攻撃で矢を使わないなら何もしない
    if (!bUseProjectileThisAttack)
    {
        return;
    }

    AUnit* Target = nullptr;

    if (PendingTarget && IsValid(PendingTarget) && !PendingTarget->bIsDead)
    {
        Target = PendingTarget;
    }
    else if (CurrentTarget && IsValid(CurrentTarget) && !CurrentTarget->bIsDead)
    {
        Target = CurrentTarget;
    }

    if (!Target)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Archer] Notify: no valid target to shoot"));
        return;
    }

    SpawnArrow(Target);
    PendingTarget = nullptr;
}
