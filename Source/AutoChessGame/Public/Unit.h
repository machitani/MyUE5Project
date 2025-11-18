#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "ItemData.h"
#include "UnitSaveData.h"
#include "EquiqSlotType.h"
#include "BoardManager.h"
#include "UnitEquiqSlot.h"
#include "UnitHoverInfoWidget.h"
#include "Unit.generated.h"

UENUM(BlueprintType)
enum class EUnitTeam : uint8
{
    Player UMETA(DisplayName = "Player"),
    Enemy  UMETA(DisplayName = "Enemy")
};

UCLASS()
class AUTOCHESSGAME_API AUnit : public AActor
{
    GENERATED_BODY()

public:
    AUnit();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USkeletalMeshComponent* UnitMesh;

    /** ドラッグ中フラグ */
    bool bIsDragging;

    /** ドラッグ開始時のマウスワールドとユニットのオフセット */
    FVector DragOffset;

    /** ドラッグ開始 */
    void StartDrag(const FVector& MouseWorld);

    /** ドラッグ終了 */
    void EndDrag();

    /** ドラッグ更新（マウスワールド位置を渡して呼ぶ） */
    void UpdateDrag(const FVector& MouseWorld);

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Unit")
    class ATile* CurrentTile;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
    FVector OriginalLocation;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Unit")
    ABoardManager* OwningBoardManager;

    //ユニットのステータス
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float MaxHP = 100.f;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,CateGory="Stats")
    float HP = 100.f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float Attack = 20.f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float MoveSpeed = 300.f;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Stats")
    float Range = 300.f;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Stats")
    int32 Level = 1;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
    float AttackInterval = 1.0f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Unit")
    bool bCanDrag = true;

    float TimeSinceLastAttack = 0.0f;

    virtual void Tick(float DeltaTime)override;

    virtual void BeginPlay()override;

    void CheckForTarget(const float DeltaTime);
    void AttackTarget(AUnit* Targer);
    void OnDeath();

    UPROPERTY()
    UUnitEquipSlot* EquipSlotRef;
    

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    EUnitTeam Team = EUnitTeam::Player;  // デフォルトはプレイヤー

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Equip")
    TMap<E_EquiqSlotType, FItemData>EquippedItems;

    UFUNCTION(BlueprintCallable,Category="Equip")
    void EquipItem(E_EquiqSlotType SlotType, const FItemData& Item);


    void ApplyItemEffect(const FItemData& Item);

    UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Animation")
    bool bIsMoving = false;

    UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Animation")
    bool bIsDead = false;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    bool bIsAttacking = false;

    UPROPERTY()
    FVector LastLocation;

    UFUNCTION()
    void UpdateAnimationState();

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Status")
    float BaseHP;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Status")
    float BaseAttack;

    UPROPERTY(BlueprintReadWrite)
    FName UnitID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    TArray<FItemData>  EquipedItems;

    UFUNCTION(BlueprintCallable,Category="Item")
    void ReapplayAllItemEffects();

    UPROPERTY()
    ATile* InitialTile;

    FUnitSaveData MakeSaveData();

    void ApplySaveData(const FUnitSaveData& Data);

    UFUNCTION()
    void OnMouseEnterUnit(UPrimitiveComponent* TouchedComponent);

    UFUNCTION()
    void OnMouseLeaveUnit(UPrimitiveComponent* TochedComponent);

    UFUNCTION()
    void UpdateHoverWidget();

    UPROPERTY()
    UUnitHoverInfoWidget* HoverWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUnitHoverInfoWidget> HoverWidgetClass;


};
