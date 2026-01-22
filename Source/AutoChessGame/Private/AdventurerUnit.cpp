// AdventurerUnit.cpp

#include "AdventurerUnit.h"
#include "PachinkoProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Animation/AnimInstance.h"

AAdventurerUnit::AAdventurerUnit()
{
    MaxHP = 85.f;
    HP = MaxHP;

    Attack = 14.f;   // 弓よりちょい低めとかお好みで
    Defense = 2.f;
    MagicDefense = 2.f;
    MagicPower = 0.f;

    Range = 520.f;  // 中距離イメージ
    MoveSpeed = 135.f;
    AttackInterval = 1.0f;  // 連射気味でも面白い

    CritChance = 0.18f;
    CritMultiplier = 1.55f;

    AttackInterval = 1.0f;
    BaseAttackInterval = 1.0f;

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

    // 距離チェック
    const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    const float MinArrowDistance = 150.f; // 好きな距離に調整

    // 近距離：矢を飛ばさず即ダメージ
    if (Distance < MinArrowDistance)
    {
        bIsAttacking = true;
        PendingTarget = Target;

        // アニメーションはちゃんと再生（近距離でも弓を引くモーションは見せる）
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

        // ★ ここで直接クリティカル付きの物理ダメージを与える
        AUnit* Attacker = this;
        float  BaseDamage = Attack * PachinkoDamageMultiplier;
        bool   bIsCritical = false;

        float FinalDamage = Attacker->CalcPhysicalDamageWithCrit(BaseDamage, bIsCritical);
        Target->bLastHitWasCritical = bIsCritical;

        Target->TakePhysicalDamage(FinalDamage);

        // この攻撃では Projectile を使わない
        return;
    }

    // --- ここから下は、今までの遠距離用ロジック（Projectile を使う） ---

    bIsAttacking = true;
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
