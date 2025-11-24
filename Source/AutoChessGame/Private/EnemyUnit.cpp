#include "EnemyUnit.h"

AEnemyUnit::AEnemyUnit()
{
    MaxHP = 120.f;
    HP = MaxHP;

    Attack = 14.f;
    Defense = 4.f;
    MagicDefense = 2.f;
    MagicPower = 0.f;

    Range = 250.f;
    MoveSpeed = 140.f;
    AttackInterval = 1.1f;

    Team = EUnitTeam::Enemy;
    UnitID = FName("Enemy");    // 必要なら敵種別で変える
}

void AEnemyUnit::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyUnit::AttackTarget(AUnit* Target)
{
    // いまは普通の物理攻撃だけ
    Super::AttackTarget(Target);

    // 将来：「爆発ロボ」「スナイパー」などに分岐させたい場合は
    // ここをさらに派生クラスでoverrideする
}
