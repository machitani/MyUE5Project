#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemData.h"
#include "EquiqSlotType.h"
#include "UnitSaveData.h"
#include "Components/WidgetComponent.h"
#include "Unit.generated.h"

class ATile;
class ABoardManager;
class UUnitHoverInfoWidget;
class UDamagePopupWidget;

UENUM(BlueprintType)
enum class EUnitTeam : uint8
{
    Player,
    Enemy
};

UCLASS()
class AUTOCHESSGAME_API AUnit : public AActor
{
    GENERATED_BODY()

public:
    AUnit();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // ====== ステータス ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float HP = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Attack = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BaseHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BaseAttack;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float BaseDefense;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float BaseMagicPower;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float BaseMagicDefense;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float BaseRange;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float BaseMoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BaseCritChance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BaseCritMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Range = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MoveSpeed = 150.f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float CritChance = 0.f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float CritMultiplier = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    EUnitTeam Team = EUnitTeam::Player;

    // ====== 防御系 / 魔法系ステータス ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Defense")
    float Defense = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Magic")
    float MagicPower = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Magic")
    float MagicDefense = 0.f;

    // ====== 攻撃間隔 ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackInterval = 1.0f;

    float TimeSinceLastAttack = 0.f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float BaseAttackInterval = 1.0f;

    // ====== メッシュ ======
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USkeletalMeshComponent* UnitMesh;

    // ====== タイル関連 ======
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    ATile* CurrentTile;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    ATile* InitialTile;

    FVector OriginalLocation;

    UPROPERTY()
    ATile* DragStartTile = nullptr;

    // ====== ボードManager ======
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    ABoardManager* OwningBoardManager;

    // ====== アイテム ======
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TArray<FItemData> EquipedItems;

    void EquipItem(E_EquiqSlotType SlotType, const FItemData& Item);
    void ApplyItemEffect(const FItemData& Item);
    void ReapplayAllItemEffects();

    UFUNCTION(BlueprintCallable,Category="Item")
    void RemoveItems();

    // ====== Hover ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UUnitHoverInfoWidget> HoverWidgetClass;

    UPROPERTY()
    UUnitHoverInfoWidget* HoverWidget;

    void ShowUnitInfo();
    void HideUnitInfo();

    // ====== 状態 ======
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")

    bool bIsMoving = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsAttacking = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsDead = false;

    // ====== Drag ======
    bool bIsDragging;
    bool bCanDrag;
    FVector DragOffset;

    void StartDrag(const FVector& MouseWorld);
    void UpdateDrag(const FVector& MouseWorld);
    void EndDrag();

    // ====== 移動・攻撃 ======
    void CheckForTarget(float DeltaTime);
    virtual void AttackTarget(AUnit* Target);

    // ====== ダメージ処理 ======
    virtual void TakePhysicalDamage(float DamageAmount);
    virtual void TakeMagicDamage(float DamageAmount);

    // ====== スキルシステム ======
    virtual bool CanUseSkill() const { return false; }
    virtual void UseSkill(AUnit* Target) {}

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Skill")
    bool bHasSkill = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SkillCooldown = 5.0f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float SkillTimer = 0.0f;

    // ====== 死亡 ======
    virtual void OnDeath();

    UFUNCTION(BlueprintCallable,Category="Death")
    void OnDeathFinished();

    // ====== アニメ ======
    void UpdateAnimationState();
    FVector LastLocation;

    // ====== セーブ ======
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
    FName UnitID;

    FUnitSaveData MakeSaveData();
    void ApplySaveData(const FUnitSaveData& Data);

    // ====== マウス ======
    UFUNCTION()
    void OnUnitClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

    // ==== ターゲット =====
    UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Combat")
    AUnit* CurrentTarget = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
    float TargetSearchRadius = 2000.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
    float TargetLostDistance = 2500.f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    bool bFaceTarget = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float RotationInterpSpeed = 10.f;       

    // ※見た目用の微調整が必要になったらこれを使う（今は 0 でOK）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float FacingYawOffset = 0.f;

    void UpdateFacing(float DeltaTime);

    UFUNCTION()
    void RefreshHoverInfo();

    //HPバー
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
    UWidgetComponent* HPBarWidget;

    UFUNCTION(BlueprintCallable,Category="UI")
    float GetHPPercent()const;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Component")
    USceneComponent* RootScene;

    // ====== ダメージポップアップ ======
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="UI")
    TSubclassOf<UDamagePopupWidget> DamagePopupWidgetClass;

    void ShowDamagePopup(float DamageAmount, bool bIsMagicDamage);

    void ShowHealPopup(float HealAmount);

    void ShowBuffPopup(const FString& Text);


    UFUNCTION(BlueprintCallable, Category = "Combat")
    float CalcPhysicalDamageWithCrit(float BaseDamage, bool& bOutIsCritical);

    // ポップアップ用に直前のヒット状態を覚えておく
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bLastHitWasCritical = false;
};


