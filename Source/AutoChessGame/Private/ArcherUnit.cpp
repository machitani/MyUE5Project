#include "ArcherUnit.h"

AArcherUnit::AArcherUnit()
{
    MaxHP = 100.f;
    HP = MaxHP;

    Attack = 16.f;
    Defense = 2.f;
    MagicDefense = 2.f;
    MagicPower = 0.f;

    Range = 420.f;   // ’·Ë’ö
    MoveSpeed = 150.f;
    AttackInterval = 0.9f;   // ‚¿‚å‚¢‘¬‚ß

    Team = EUnitTeam::Player;
    UnitID = FName("Archer");
}

void AArcherUnit::BeginPlay()
{
    Super::BeginPlay();
}

void AArcherUnit::AttackTarget(AUnit* Target)
{
    // «—ˆF–î‚ÌProjectile‚ğ”ò‚Î‚·ˆ—‚ğ‚±‚±‚É’Ç‰Á—\’è
    Super::AttackTarget(Target);
}
