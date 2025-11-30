// Fill out your copyright notice in the Description page of Project Settings.


#include "SuportEnemy.h"
#include "EngineUtils.h"

ASuportEnemy::ASuportEnemy()
{
    MaxHP = 160.f;
    HP = MaxHP;

    Attack = 12.f;   // 低め
    Defense = 5.f;
    MagicDefense = 5.f;
    MagicPower = 10.f;   // 将来、魔法攻撃に使いたかったらココ

    Range = 500.f;  // 中距離
    MoveSpeed = 130.f;
    AttackInterval = 1.2f;

    Team = EUnitTeam::Enemy;
    UnitID = FName("SupportEnemy");

    // スキル設定（ヒール）
    bHasSkill = true;
    SkillCooldown = 5.0f;   // 5秒ごとにヒール
    SkillTimer = 0.0f;

    HealAmount = 25.f;
    HealRadius = 600.f;

    // ターゲット方向を向く
    bFaceTarget = true;
    FacingYawOffset = 0.f;
    RotationInterpSpeed = 10.f;
}

void ASuportEnemy::BeginPlay()
{
    Super::BeginPlay();
}

bool ASuportEnemy::CanUseSkill() const
{
    return bHasSkill && (SkillTimer >= SkillCooldown);
}

void ASuportEnemy::UseSkill(AUnit* Target)
{
    if (!GetWorld()) return;

    AUnit* BestAlly = nullptr;
    float  LowestHpRatio = 1.1f;  // 1より大きい値で初期化

    // 一番HP割合が低い味方を探す
    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Other = *It;
        if (!Other) continue;
        if (Other == this) continue;
        if (Other->Team != Team) continue;
        if (Other->bIsDead || Other->HP <= 0.f) continue;
        if (Other->MaxHP <= 0.f) continue;

        const float Dist = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
        if (Dist > HealRadius) continue;

        const float HpRatio = Other->HP / Other->MaxHP;
        if (HpRatio >= 1.0f) continue;  // 全快は対象外

        if (HpRatio < LowestHpRatio)
        {
            LowestHpRatio = HpRatio;
            BestAlly = Other;
        }
    }

    if (!BestAlly) return;

    // 回復
    BestAlly->HP = FMath::Min(BestAlly->HP + HealAmount, BestAlly->MaxHP);

    UE_LOG(LogTemp, Warning,
        TEXT("[SupportEnemy] Healed ally %s by %.1f (HP=%.1f / %.1f)"),
        *BestAlly->GetName(),
        HealAmount,
        BestAlly->HP,
        BestAlly->MaxHP
    );

    // クールタイムリセット
    SkillTimer = 0.f;
}

void ASuportEnemy::AttackTarget(AUnit* Target)
{
    Super::AttackTarget(Target);
}
