#include "WizardUnit.h"
#include "FireballProfectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"

AWizardUnit::AWizardUnit()
{
    MaxHP = 85.f;
    HP = MaxHP;

    Attack = 6.f;
    Defense = 2.f;
    MagicDefense = 3.f;
    MagicPower = 20.f;

    Range = 560.f;
    MoveSpeed = 120.f;
    AttackInterval = 1.3f;

    BaseAttackInterval = 1.0f;

    Team = EUnitTeam::Player;
    UnitID = FName("Wizard");
}

void AWizardUnit::BeginPlay()
{
    Super::BeginPlay();
}

// 今は通常攻撃だけ使うのでスキルはオフ
bool AWizardUnit::CanUseSkill() const
{
    return false;
}

void AWizardUnit::UseSkill(AUnit* Target)
{
    // 後でスキル作りたくなったらここに書く
}

void AWizardUnit::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

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

                if (!AnimInstance->Montage_IsPlaying(AttackMontage))
                {
                    bFiredThisAttack = false;
                    AnimInstance->Montage_Play(AttackMontage, PlayRate);
                }
            }
        }
    }
    bFiredThisAttack = false;
}

void AWizardUnit::SpawnFireball(AUnit* Target)
{
    if (!FireballClass || !Target) return;

    UWorld* World = GetWorld();
    if (!World) return;

    const FVector SpawnLocation = GetActorLocation() + FireballSpawnOffset;

    FVector TargetLocation = Target->GetActorLocation();
    TargetLocation.Z = SpawnLocation.Z;

    FVector Dir = (TargetLocation - SpawnLocation);
    Dir.Z = 0.f;
    Dir.Normalize();

    const FRotator SpawnRotation = Dir.Rotation();

    FActorSpawnParameters Params;
    Params.Owner = this;

    AFireballProfectile* P = World->SpawnActor<AFireballProfectile>(
        FireballClass, SpawnLocation, SpawnRotation, Params);

    if (P && P->ProjectileMovement)
    {
        const float MagicDamage = MagicPower * MagicAttackMultiplier;

        P->TargetUnit = Target;
        P->DamageAmount = MagicDamage;
        P->OwnerTeam = Team;

        P->ProjectileMovement->Velocity =
            Dir * P->ProjectileMovement->InitialSpeed;
    }
}

void AWizardUnit::HandleAttackEndNotify()
{
    bIsAttacking = false;
    bFiredThisAttack = false;
}

// ★ AnimNotify から呼ぶ用
void AWizardUnit::HandleFireballShootNotify()
{
    if (bFiredThisAttack)return;
    bFiredThisAttack = true;
    // 攻撃中にターゲットが死んでたら何もしない
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead)
    {
        return;
    }

    SpawnFireball(PendingTarget);
}
