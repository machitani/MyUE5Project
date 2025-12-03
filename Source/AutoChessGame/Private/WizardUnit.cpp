#include "WizardUnit.h"
#include "FireballProfectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"

AWizardUnit::AWizardUnit()
{
    MaxHP = 90.f;
    HP = MaxHP;

    Attack = 8.f;
    Defense = 1.f;
    MagicDefense = 4.f;
    MagicPower = 28.f;

    Range = 320.f;
    MoveSpeed = 130.f;
    AttackInterval = 1.3f;

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

    // この攻撃で狙っている敵を覚えておく
    PendingTarget = Target;

    // 攻撃アニメーション再生
    if (UnitMesh)
    {
        if (UAnimInstance* AnimInstance = UnitMesh->GetAnimInstance())
        {
            if (AttackMontage)
            {
                // 連打で再生されすぎるのを防ぐならチェック入れてもOK
                if (!AnimInstance->Montage_IsPlaying(AttackMontage))
                {
                    AnimInstance->Montage_Play(AttackMontage);
                }
            }
        }
    }

    // ★ ここでは弾を出さない
    // 実際の発射は AnimNotify → HandleFireballShootNotify で行う
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

// ★ AnimNotify から呼ぶ用
void AWizardUnit::HandleFireballShootNotify()
{
    // 攻撃中にターゲットが死んでたら何もしない
    if (!PendingTarget || !IsValid(PendingTarget) || PendingTarget->bIsDead)
    {
        return;
    }

    SpawnFireball(PendingTarget);
}
