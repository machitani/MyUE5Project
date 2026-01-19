#include "SniperEnemy.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"


ASniperEnemy::ASniperEnemy()
{
    MaxHP = 180.f;
    HP = MaxHP;

    Attack = 55.f;          // 単体火力高め
    Defense = 4.f;
    MagicDefense = 3.f;
    MagicPower = 0.f;

    Range = 900.f;          // 通常でも長射程
    MoveSpeed = 110.f;      // ちょい遅
    AttackInterval = 1.9f;  // 発射間隔長め

    Team = EUnitTeam::Enemy;
    UnitID = FName("SniperEnemy");

    // スキル（スナイプ単体大ダメ）
    bHasSkill = true;
    SkillCooldown = 7.0f;   // 7秒ごとにスナイプ
    SkillTimer = 0.0f;

    SnipeDamageMultiplier = 2.2f;
    SnipeExtraRange = 600.f;
    SnipeKnockbackStrength = 0.f; // 0なら無し

    // ターゲットを向く
    bFaceTarget = true;
    FacingYawOffset = 0.f;
    RotationInterpSpeed = 12.f;
}

void ASniperEnemy::BeginPlay()
{
    Super::BeginPlay();
}

bool ASniperEnemy::CanUseSkill() const
{
    return bHasSkill && (SkillTimer >= SkillCooldown);
}

void ASniperEnemy::UseSkill(AUnit* Target)
{
    if (!GetWorld() || !Target) return;
    if (Target->bIsDead || Target->HP <= 0.f) return;

    // 通常射程よりさらに遠くでも撃てるチェック
    const float MaxSnipeRange = Range + SnipeExtraRange;
    const float Dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Dist > MaxSnipeRange)
    {
        return; // 遠すぎるなら不発
    }

    const float Damage = Attack * SnipeDamageMultiplier;
    Target->TakePhysicalDamage(Damage);

    // ノックバック入れたい場合（Unit側がCharacter想定のとき）
    if (SnipeKnockbackStrength > 0.f)
    {
        const FVector Dir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        if (ACharacter* Ch = Cast<ACharacter>(Target))
        {
            Ch->LaunchCharacter(Dir * SnipeKnockbackStrength, true, true);
        }
    }

    UE_LOG(LogTemp, Warning,
        TEXT("[SniperEnemy] Snipe shot! Target=%s Damage=%.1f Dist=%.1f"),
        *GetNameSafe(Target), Damage, Dist);

    // クールタイムリセット
    SkillTimer = 0.f;
}

void ASniperEnemy::AttackTarget(AUnit* Target)
{
    // スキル優先で撃ちたいならここで分岐
    if (Target && CanUseSkill())
    {
        UseSkill(Target);
        return;
    }

    Super::AttackTarget(Target);
}
