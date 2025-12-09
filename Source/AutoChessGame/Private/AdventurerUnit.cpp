// AdventurerUnit.cpp

#include "AdventurerUnit.h"
#include "PachinkoProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Animation/AnimInstance.h"

AAdventurerUnit::AAdventurerUnit()
{
    MaxHP = 100.f;
    HP = MaxHP;

    Attack = 14.f;   // 弓よりちょい低めとかお好みで
    Defense = 3.f;
    MagicDefense = 2.f;
    MagicPower = 0.f;

    Range = 350.f;  // 中距離イメージ
    MoveSpeed = 150.f;
    AttackInterval = 1.0f;  // 連射気味でも面白い

    Team = EUnitTeam::Player;
    UnitID = FName("Adventurer");
}

void AAdventurerUnit::BeginPlay()
{
    Super::BeginPlay();
}

bool AAdventurerUnit::CanUseSkill() const
{
    return false; // 今は通常攻撃だけ。後でスキル入れる枠
}

void AAdventurerUnit::UseSkill(AUnit* Target)
{
    // スキル作りたくなったらここに
}

void AAdventurerUnit::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

    // 攻撃中フラグON（AnimBPでアタックモーション再生に使う）
    bIsAttacking = true;

    // この攻撃で狙っている敵を保存（Notifyから使う）
    PendingTarget = Target;
}

void AAdventurerUnit::SpawnPachinkoShot(AUnit* Target)
{
    if (!PachinkoClass || !Target) return;

    UWorld* World = GetWorld();
    if (!World) return;

    const FVector SpawnLocation = GetActorLocation() + PachinkoSpawnOffset;

    FVector TargetLocation = Target->GetActorLocation();
    TargetLocation.Z = SpawnLocation.Z;

    FVector Dir = (TargetLocation - SpawnLocation);
    Dir.Z = 0.f;
    Dir.Normalize();

    const FRotator SpawnRotation = Dir.Rotation();

    FActorSpawnParameters Params;
    Params.Owner = this;

    APachinkoProjectile* P = World->SpawnActor<APachinkoProjectile>(
        PachinkoClass, SpawnLocation, SpawnRotation, Params);

    if (P && P->ProjectileMovement)
    {
        const float PhysicalDamage = Attack * PachinkoDamageMultiplier;

        P->TargetUnit = Target;
        P->DamageAmount = PhysicalDamage;
        P->OwnerTeam = Team;

        P->ProjectileMovement->Velocity =
            Dir * P->ProjectileMovement->InitialSpeed;
    }
}

void AAdventurerUnit::HandlePachinkoShootNotify()
{
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead)
    {
        return;
    }

    SpawnPachinkoShot(PendingTarget);
}
