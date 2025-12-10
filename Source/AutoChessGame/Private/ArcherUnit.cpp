// ArcherUnit.cpp

#include "ArcherUnit.h"
#include "ArrowProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Animation/AnimInstance.h"

AArcherUnit::AArcherUnit()
{
    MaxHP = 80.f;
    HP = MaxHP;

    Attack = 18.f;    // 物理高め
    Defense = 2.f;
    MagicDefense = 1.f;
    MagicPower = 0.f;

    Range = 400.f;  // Wizard よりちょい長めでもいい
    MoveSpeed = 140.f;
    AttackInterval = 1.1f;
    BaseAttackInterval = 1.0f;

    Team = EUnitTeam::Player;
    UnitID = FName("Archer");

    CritChance = 0.25f;   // 25% くらい
    CritMultiplier = 1.5f;
}

void AArcherUnit::BeginPlay()
{
    Super::BeginPlay();
}

bool AArcherUnit::CanUseSkill() const
{
    return false; // まずは通常攻撃だけ
}

void AArcherUnit::UseSkill(AUnit* Target)
{
    // 後でスキルを作りたくなったらここに
}

// ArcherUnit.cpp

void AArcherUnit::AttackTarget(AUnit* Target)
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
        float  BaseDamage = Attack * ArrowDamageMultiplier;
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

    // 矢はアニメーション Notify → HandleArrowShootNotify() → SpawnArrow() で飛ぶ
}


void AArcherUnit::SpawnArrow(AUnit* Target)
{
    if (!ArrowClass || !Target) return;

    UE_LOG(LogTemp, Warning, TEXT("ARROW"));

    UWorld* World = GetWorld();
    if (!World) return;

    const FVector SpawnLocation = GetActorLocation() + ArrowSpawnOffset;

    FVector TargetLocation = Target->GetActorLocation();
    TargetLocation.Z = SpawnLocation.Z;

    FVector Dir = (TargetLocation - SpawnLocation);
    Dir.Z = 0.f;
    Dir.Normalize();

    const FRotator SpawnRotation = Dir.Rotation();

    FActorSpawnParameters Params;
    Params.Owner = this;

    AArrowProjectile* P = World->SpawnActor<AArrowProjectile>(
        ArrowClass, SpawnLocation, SpawnRotation, Params);

    if (P && P->ProjectileMovement)
    {
        const float PhysicalDamage = Attack * ArrowDamageMultiplier;

        P->TargetUnit = Target;
        P->DamageAmount = PhysicalDamage;
        P->OwnerTeam = Team;

        P->ProjectileMovement->Velocity =
            Dir * P->ProjectileMovement->InitialSpeed;
    }
}

void AArcherUnit::HandleArrowShootNotify()
{
    // ログ入れておくとデバッグしやすい
    UE_LOG(LogTemp, Warning,
        TEXT("[Archer] Notify fired. Pending=%s"),
        PendingTarget ? *PendingTarget->GetName() : TEXT("NULL"));

    AUnit* Target = nullptr;

    // ① 優先：AttackTarget でロックした PendingTarget
    if (PendingTarget && IsValid(PendingTarget) && !PendingTarget->bIsDead)
    {
        Target = PendingTarget;
    }
    // ② 予備：まだ生きてる別の CurrentTarget がいればそっちに撃つ
    else if (CurrentTarget && IsValid(CurrentTarget) && !CurrentTarget->bIsDead)
    {
        Target = CurrentTarget;
    }

    if (!Target)
    {
        // 完全にターゲットがいない → このフレームは矢を出さない
        UE_LOG(LogTemp, Warning, TEXT("[Archer] Notify: no valid target to shoot"));
        return;
    }

    SpawnArrow(Target);

    // 1回撃ったら PendingTarget はクリア
    PendingTarget = nullptr;
}
