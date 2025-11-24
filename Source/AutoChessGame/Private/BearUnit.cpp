#include "BearUnit.h"

ABearUnit::ABearUnit()
{
    // タンク寄りステータス
    MaxHP = 220.f;
    HP = MaxHP;

    Attack = 14.f;
    Defense = 10.f;     // 物理にかなり強い
    MagicDefense = 4.f;
    MagicPower = 0.f;

    Range = 200.f;
    MoveSpeed = 120.f;
    AttackInterval = 1.2f;

    Team = EUnitTeam::Player;
    UnitID = FName("Bear");
}

void ABearUnit::BeginPlay()
{
    Super::BeginPlay();
}

void ABearUnit::AttackTarget(AUnit* Target)
{
    if (!Target) return;

    // 今はシンプルに物理攻撃のみ
    Super::AttackTarget(Target);

    // 将来：
    // ・攻撃時に一瞬防御アップ
    // ・HPが減るほど攻撃アップ
    // などのパッシブをここに足していける
}
