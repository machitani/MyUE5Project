// EnemyAssaultProjectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.h"   // AUnit / EUnitTeam
#include "EnemyAssaultProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class AUTOCHESSGAME_API AEnemyAssaultProjectile : public AActor
{
    GENERATED_BODY()

public:
    AEnemyAssaultProjectile();

    virtual void Tick(float DeltaTime) override;

    // 攻撃先（プレイヤーユニット）を覚えておく
    UPROPERTY(BlueprintReadWrite, Category = "Assault")
    AUnit* TargetUnit = nullptr;

    // 敵アサルトが計算して渡す最終ダメージ
    UPROPERTY(BlueprintReadWrite, Category = "Assault")
    float DamageAmount = 0.f;

    // この弾の発射元チーム（= Enemy）
    UPROPERTY(BlueprintReadWrite, Category = "Assault")
    EUnitTeam OwnerTeam = EUnitTeam::Enemy;

    // どのくらい近づいたら「当たり」とみなすか
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assault")
    float HitRadius = 40.f;

protected:
    virtual void BeginPlay() override;

public:
    // コリジョン
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* BulletCollision;

    // 見た目（ちっちゃい弾メッシュ）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;

    // 弾の移動
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProjectileMovementComponent* ProjectileMovement;
};
