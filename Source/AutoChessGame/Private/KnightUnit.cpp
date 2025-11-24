#include "KnightUnit.h"

AKnightUnit::AKnightUnit()
{
    // タンク寄りステータス
    MaxHP = 180.f;
    HP = MaxHP;

    Attack = 18.f;
    Defense = 8.f;     // 物理に強い
    MagicDefense = 3.f;
    MagicPower = 0.f;

    Range = 200.f;
    MoveSpeed = 140.f;
    AttackInterval = 1.1f;

    Team = EUnitTeam::Player;
    UnitID = FName("Knight");
}

void AKnightUnit::BeginPlay()
{
    Super::BeginPlay();
}

void AKnightUnit::AttackTarget(AUnit* Target)
{
    // 今は基本攻撃だけ（物理）
    Super::AttackTarget(Target);

    // 将来：シールドバッシュとか入れるならここ
}
