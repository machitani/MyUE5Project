#include "Unit.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "ItemData.h"
#include "UnitSaveData.h"
#include "Tile.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "CustomPlayerController.h"
#include "UnitEquiqSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Components/VerticalBox.h"


AUnit::AUnit()
{
    PrimaryActorTick.bCanEverTick = true;

    UnitMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UnitMesh"));
    RootComponent = UnitMesh;

    bIsDragging = false;
    DragOffset = FVector::ZeroVector;

    UnitMesh->SetMobility(EComponentMobility::Movable);
    UnitMesh->SetGenerateOverlapEvents(true);
    UnitMesh->bSelectable = true;

    CurrentTile = nullptr;
    OriginalLocation = FVector::ZeroVector;
    bCanDrag = true;
    TimeSinceLastAttack = 0.0f;
}

void AUnit::StartDrag(const FVector& MouseWorld)
{
    if (!bCanDrag) return;

    bIsDragging = true;
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    DragOffset = GetActorLocation() - MouseWorld;
}

void AUnit::EndDrag()
{
    bIsDragging = false;
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AUnit::UpdateDrag(const FVector& MouseWorld)
{
    if (!bIsDragging) return;

    FVector Target = MouseWorld + DragOffset;
    Target.Z = GetActorLocation().Z; // 高さ維持
    SetActorLocation(Target);
}

void AUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (EquipSlotRef)
    {
        FVector WorldPos = GetActorLocation() + FVector(0, 0, 200);
        FVector2D ScreenPos;
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        UGameplayStatics::ProjectWorldToScreen(PC, WorldPos, ScreenPos);
        EquipSlotRef->SetPositionInViewport(ScreenPos);
    }

    if (bIsDragging)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            FVector MouseWorld, MouseDir;
            if (PC->DeprojectMousePositionToWorld(MouseWorld, MouseDir))
            {
                UpdateDrag(MouseWorld);
            }
        }
        return;
    }

    if (OwningBoardManager && OwningBoardManager->CurrentPhase == EGamePhase::Battle)
    {
        CheckForTarget(DeltaTime);
    }

    UpdateAnimationState();
}

void AUnit::BeginPlay()
{
    Super::BeginPlay();

    BaseHP = HP;
    BaseAttack = Attack;

    LastLocation = GetActorLocation();

    UnitMesh->OnBeginCursorOver.AddDynamic(this, &AUnit::OnMouseEnterUnit);
    UnitMesh->OnEndCursorOver.AddDynamic(this, &AUnit::OnMouseLeaveUnit);

    //プレイヤーコントローラー取得
    ACustomPlayerController* PC = Cast<ACustomPlayerController>(GetWorld()->GetFirstPlayerController());
    if (!PC || !PC->EquipSlotClass) return;

    //スロット生成
    UUnitEquipSlot* EquipSlot = CreateWidget<UUnitEquipSlot>(PC, PC->EquipSlotClass);
    if (EquipSlot)
    {
        EquipSlot->OwnerUnit = this;
        EquipSlot->SlotType = E_EquiqSlotType::Weapon;
        EquipSlot->AddToViewport();

        // --- 初期位置（ユニットの頭上） ---
        FVector WorldPos = GetActorLocation() + FVector(0, 0, 10);
        FVector2D ScreenPos;
        UGameplayStatics::ProjectWorldToScreen(PC, WorldPos, ScreenPos);
        EquipSlot->SetPositionInViewport(ScreenPos);

        // --- 参照保持（後で位置追従などに使用） ---
        EquipSlotRef = EquipSlot;
    }
}

void AUnit::CheckForTarget(const float DeltaTime)
{
    if (!OwningBoardManager) return;
    if (OwningBoardManager->CurrentPhase != EGamePhase::Battle) return;

    // 攻撃クールダウン更新
    TimeSinceLastAttack += DeltaTime;

    if (TimeSinceLastAttack < AttackInterval) return;

    // 最も近い敵を探索
    AUnit* ClosestEnemy = nullptr;
    float ClosestDist = FLT_MAX;

    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Other = *It;
        if (Other == this) continue;
        if (Other->Team == Team) continue;
        if (Other->HP <= 0.f) continue;

        float Distance = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
        if (Distance < ClosestDist)
        {
            ClosestDist = Distance;
            ClosestEnemy = Other;
        }
    }

    if (!ClosestEnemy) return;

    if (ClosestDist <= Range)
    {
        // 射程内なら攻撃
        AttackTarget(ClosestEnemy);
        TimeSinceLastAttack = 0.f;
    }
    else
    {
        // 射程外なら移動
        FVector NewLocation = FMath::VInterpConstantTo(
            GetActorLocation(),
            ClosestEnemy->GetActorLocation(),
            DeltaTime,
            MoveSpeed // 1秒あたりの移動距離
        );
        SetActorLocation(NewLocation);
    }
}

void AUnit::AttackTarget(AUnit* Target)
{
    if (!Target) return;

    bIsAttacking = true;

    Target->HP -= Attack;

    UE_LOG(LogTemp, Warning, TEXT("%s attacked %s (HP: %.1f)"),
        *GetName(), *Target->GetName(), Target->HP);

    if (Target->HP <= 0.f)
    {
        Target->OnDeath();
    }

    //bIsAttacking = false;
}

void AUnit::OnDeath()
{
    UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());

    bIsAttacking = false;
    bIsMoving = false;
    bIsDead = true;

    // UI（EquipSlot）も消す
    if (EquipSlotRef)
    {
        EquipSlotRef->RemoveFromParent();
        EquipSlotRef = nullptr;
    }

    if (CurrentTile)
    {
        CurrentTile->bIsOccupied = false;
        CurrentTile->OccupiedUnit = nullptr;
        //CurrentTile = nullptr;
    }

    // BoardManager の配列から削除
    /*if (OwningBoardManager)
    {
        if (Team == EUnitTeam::Player)
            OwningBoardManager->PlayerUnits.Remove(this);
        else
            OwningBoardManager->EnemyUnits.Remove(this);
    }*/

    Destroy();   // ← これが重要！
}


void AUnit::EquipItem(E_EquiqSlotType SlotType, const FItemData& Item)
{
    UE_LOG(LogTemp, Warning, TEXT("GET ITEM"))

        EquipedItems.Add(Item);

        ApplyItemEffect(Item);
}

void AUnit::ApplyItemEffect(const FItemData& Item)
{
    if (Item.EffectType == "Attack")
    {
        UE_LOG(LogTemp, Warning, TEXT("ATTACK"))
        Attack += Item.EffectValue;
        
    }
    if (Item.EffectType == "HP")
    {
        HP += Item.EffectValue;
    }
}

void AUnit::UpdateAnimationState()
{
    // 移動判定
    FVector Now = GetActorLocation();
    float Distance = FVector::Dist(Now, LastLocation);
    bIsMoving = (Distance > 0.1f);
    LastLocation = Now;

    // 攻撃中の判定は Tick で触らない
    // bIsAttacking は AttackTarget 内で管理される

    // 死亡状態の設定
    if (HP <= 0.f)
    {
        //bIsDead = true;
    }
}

void AUnit::ReapplayAllItemEffects()
{

    HP = BaseHP;
    Attack = BaseAttack;

    for (const FItemData& Item : EquipedItems)
    {
        ApplyItemEffect(Item);
    }
}

FUnitSaveData AUnit::MakeSaveData()
{
    FUnitSaveData Data;
    Data.UnitID = UnitID;
    Data.BaseHP = BaseHP;
    Data.BaseAttack = BaseAttack;
    Data.EquippedItems = EquipedItems;
    if (CurrentTile && OwningBoardManager)
    {
        Data.SavedTileIndex = OwningBoardManager
             ->PlayerTiles.IndexOfByKey(CurrentTile);
    }
     else
     {
        Data.SavedTileIndex = -1;
     }

    return Data;
}

void AUnit::ApplySaveData(const FUnitSaveData& Data)
{
    BaseHP = Data.BaseHP;
    BaseAttack = Data.BaseAttack;
    EquipedItems = Data.EquippedItems;

    HP = BaseHP;
    Attack = BaseAttack;
    for (auto& Item : EquipedItems)
    {
        ApplyItemEffect(Item);
    }
}

void AUnit::OnMouseEnterUnit(UPrimitiveComponent* TouchedComponent)
{
    if (bIsDead) return;

    UE_LOG(LogTemp, Warning, TEXT("UNIT ID"));

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC || !HoverWidgetClass) return;

    if (!HoverWidget)
    {
        HoverWidget = CreateWidget<UUnitHoverInfoWidget>(PC, HoverWidgetClass);
        HoverWidget->AddToViewport();
    }

    // データ反映
    HoverWidget->SetUnitInfo(UnitID, HP, Attack, EquipedItems);
}

void AUnit::OnMouseLeaveUnit(UPrimitiveComponent* TouchedComponent)
{
    if (HoverWidget)
    {
        HoverWidget->RemoveFromParent();
        HoverWidget = nullptr;
    }
}

void AUnit::UpdateHoverWidget()
{
   
}
