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

    Team = EUnitTeam::Player;
    UnitID = FName("Archer");
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

void AArcherUnit::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    // 攻撃中フラグON（AnimBPでアタックモーション再生に使う）
    bIsAttacking = true;

    // この攻撃で狙っている敵を保存（Notifyから使う）
    PendingTarget = Target;
}

void AArcherUnit::SpawnArrow(AUnit* Target)
{
    if (!ArrowClass || !Target) return;

    UE_LOG(LogTemp,Warning,TEXT("ARROW"))

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
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead)
    {
        return;
    }

    SpawnArrow(PendingTarget);
}
