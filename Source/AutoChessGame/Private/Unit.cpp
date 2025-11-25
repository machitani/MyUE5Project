#include "Unit.h"
#include "Tile.h"
#include "BoardManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EngineUtils.h"
#include "UnitHoverInfoWidget.h"

AUnit::AUnit()
{
    PrimaryActorTick.bCanEverTick = true;

    UnitMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UnitMesh"));
    RootComponent = UnitMesh;

    UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    UnitMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    UnitMesh->OnClicked.AddDynamic(this, &AUnit::OnUnitClicked);

    bIsDragging = false;
    bCanDrag = true;
}

void AUnit::BeginPlay()
{
    Super::BeginPlay();

    BaseHP = HP;
    BaseAttack = Attack;
    BaseDefense = Defense;
    BaseMagicPower = MagicPower;
    BaseMagicDefense = MagicDefense;

    LastLocation = GetActorLocation();

    
}

void AUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

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

void AUnit::StartDrag(const FVector& MouseWorld)
{
    if (!bCanDrag || bIsDragging) return;

    bIsDragging = true;
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    OriginalLocation = GetActorLocation();

    if (CurrentTile)
    {
        CurrentTile->bIsOccupied = false;
        CurrentTile->OccupiedUnit = nullptr;
        CurrentTile = nullptr;
    }

    DragOffset = GetActorLocation() - MouseWorld;
}
void AUnit::UpdateDrag(const FVector& MouseWorld)
{
    if (!bIsDragging) return;

    FVector Target = MouseWorld + DragOffset;
    Target.Z = GetActorLocation().Z;

    SetActorLocation(Target);
}

void AUnit::EndDrag()
{
    bIsDragging = false;
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    if (!OwningBoardManager)
    {
        SetActorLocation(OriginalLocation);
        return;
    }

    FVector UnitLoc = GetActorLocation();
    ATile* Tile = OwningBoardManager->GetTileUnderLocation(UnitLoc);

    if (Tile)
    {
        OwningBoardManager->MoveUnitToTile(this, Tile);
    }
    else
    {
        SetActorLocation(OriginalLocation);
    }
}

void AUnit::CheckForTarget(float DeltaTime)
{
    TimeSinceLastAttack += DeltaTime;
    if (TimeSinceLastAttack < AttackInterval) return;

    AUnit* ClosestEnemy = nullptr;
    float ClosestDist = FLT_MAX;

    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Other = *It;
        if (Other == this) continue;
        if (Other->Team == Team) continue;
        if (Other->HP <= 0.f) continue;

        float Dist = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
        if (Dist < ClosestDist)
        {
            ClosestDist = Dist;
            ClosestEnemy = Other;
        }
    }

    if (!ClosestEnemy) return;

    if (ClosestDist <= Range)
    {
        AttackTarget(ClosestEnemy);
        TimeSinceLastAttack = 0.f;
    }
    else
    {
        FVector NewLoc = FMath::VInterpConstantTo(
            GetActorLocation(),
            ClosestEnemy->GetActorLocation(),
            DeltaTime,
            MoveSpeed
        );
        SetActorLocation(NewLoc);
    }
}

void AUnit::AttackTarget(AUnit* Target)
{
    if (!Target) return;

    bIsAttacking = true;

    // 物理攻撃（基本攻撃）
    Target->TakePhysicalDamage(Attack);

    // スキルが使えるなら発動
    if (CanUseSkill())
    {
        UseSkill(Target);
    }
}

void AUnit::TakePhysicalDamage(float DamageAmount)
{
    float FinalDamage = FMath::Max(1.f, DamageAmount - Defense);
    HP -= FinalDamage;

    UE_LOG(LogTemp, Warning, TEXT("%s Takes Physical Damage: %.1f"), *GetName(), FinalDamage);

    if (HP <= 0.f) OnDeath();
}

void AUnit::TakeMagicDamage(float DamageAmount)
{
    float FinalDamage = FMath::Max(1.f, DamageAmount - MagicDefense);
    HP -= FinalDamage;

    UE_LOG(LogTemp, Warning, TEXT("%s Takes Magic Damage: %.1f"), *GetName(), FinalDamage);

    if (HP <= 0.f) OnDeath();
}

void AUnit::OnDeath()
{
    bIsDead = true;
    bIsAttacking = false;
    bIsMoving = false;

    if (CurrentTile)
    {
        CurrentTile->bIsOccupied = false;
        CurrentTile->OccupiedUnit = nullptr;
    }

    Destroy();
}

void AUnit::EquipItem(E_EquiqSlotType SlotType, const FItemData& Item)
{
    EquipedItems.Add(Item);
    ApplyItemEffect(Item);
}

void AUnit::ApplyItemEffect(const FItemData& Item)
{
    if (Item.EffectType == "Attack") Attack += Item.EffectValue;
    if (Item.EffectType == "HP") HP += Item.EffectValue;
}

void AUnit::ReapplayAllItemEffects()
{
    HP = BaseHP;
    Attack = BaseAttack;
    Defense = BaseDefense;
    MagicPower = BaseMagicPower;
    MagicDefense = BaseMagicDefense;

    for (auto& Item : EquipedItems)
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
        Data.SavedTileIndex =
            OwningBoardManager->PlayerTiles.IndexOfByKey(CurrentTile);
    }
    else Data.SavedTileIndex = -1;

    return Data;
}

void AUnit::ApplySaveData(const FUnitSaveData& Data)
{
    BaseHP = Data.BaseHP;
    BaseAttack = Data.BaseAttack;
    BaseDefense = Data.BaseDefense;
    BaseMagicPower = Data.BaseMagicPower;
    BaseMagicDefense = Data.BaseMagicDefense;

    EquipedItems = Data.EquippedItems;

    //一旦素の値を反映
    HP = BaseHP;
    Attack = BaseAttack;
    Defense = BaseDefense;
    MagicPower = BaseMagicPower;
    MagicDefense = BaseMagicDefense;

    for (auto& Item : EquipedItems)
    {
        ApplyItemEffect(Item);
    }
}

void AUnit::ShowUnitInfo()
{
    if (bIsDead) return;

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC || !HoverWidgetClass) return;

    // ① すでに画面に出ているホバーUIを全部消す
    {
        TArray<UUserWidget*> Existing;
        UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
            this,                                      // WorldContextObject
            Existing,                                  // 結果を入れる配列
            UUnitHoverInfoWidget::StaticClass(),       // 探したいWidgetクラス
            false                                      // TopLevelOnly -> falseでOK
        );

        for (UUserWidget* W : Existing)
        {
            if (W)
            {
                W->RemoveFromParent();
            }
        }
    }

    // ② このユニット用のホバーUIを1枚だけ作る
    HoverWidget = CreateWidget<UUnitHoverInfoWidget>(PC, HoverWidgetClass);
    if (HoverWidget)
    {
        HoverWidget->SetUnitInfo(UnitID, HP, Attack, Defense, MagicPower, MagicDefense, EquipedItems);
        HoverWidget->AddToViewport();
    }
}

void AUnit::HideUnitInfo()
{
    if (HoverWidget)
    {
        HoverWidget->RemoveFromParent();
        HoverWidget = nullptr;
    }
}





void AUnit::UpdateAnimationState()
{
    FVector Now = GetActorLocation();
    bIsMoving = (FVector::Dist(Now, LastLocation) > 0.1f);
    LastLocation = Now;
}

void AUnit::OnUnitClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
    if (ButtonPressed == EKeys::RightMouseButton)
    {
        ShowUnitInfo();
    }
}
