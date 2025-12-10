#include "Unit.h"
#include "Tile.h"
#include "DamagePopupWidget.h"
#include "BoardManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BoardManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "UnitHoverInfoWidget.h"
#include "UnitHPBarWidget.h"
#include "Components/WidgetComponent.h"

AUnit::AUnit()
{
    PrimaryActorTick.bCanEverTick = true;

    // ★ まず空のルートを作って、それをRootComponentにする
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    // ☆ メッシュは RootScene の子
    UnitMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UnitMesh"));
    UnitMesh->SetupAttachment(RootComponent);

    UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    UnitMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    UnitMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    UnitMesh->OnClicked.AddDynamic(this, &AUnit::OnUnitClicked);

    // ☆ HPバーも RootScene の子（メッシュの子じゃない）
    HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
    HPBarWidget->SetupAttachment(RootComponent);

    HPBarWidget->SetWidgetSpace(EWidgetSpace::World);
    HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
    HPBarWidget->SetDrawSize(FVector2D(120.f, 15.f));
    HPBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    HPBarWidget->SetCastShadow(false);
    HPBarWidget->bCastInsetShadow = false;

    HPBarWidget->SetUsingAbsoluteRotation(true); // お好みで

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
    BaseRange = Range;
    BaseMoveSpeed = MoveSpeed;

    MaxHP = BaseHP;

    //BaseAttackInterval = AttackInterval;

    LastLocation = GetActorLocation();

    if (HPBarWidget)
    {
        if (UUserWidget* UW = HPBarWidget->GetUserWidgetObject())
        {
            if (UUnitHPBarWidget* HPBar = Cast<UUnitHPBarWidget>(UW))
            {
                HPBar->OwnerUnit = this;
            }
        }
    }
}

void AUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 死亡中はAIもドラッグも止める
    if (bIsDead)
    {
        return;
    }

    // ドラッグ中は常にマウス追従だけ
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

    // スキルタイマー進行（今は使ってなくてもOK）
    if (bHasSkill)
    {
        SkillTimer = FMath::Min(SkillTimer + DeltaTime, SkillCooldown);
    }

    // バトル中だけAIを動かす
    if (OwningBoardManager && OwningBoardManager->CurrentPhase == EGamePhase::Battle)
    {
        CheckForTarget(DeltaTime);
        //UpdateFacing(DeltaTime);
    }
    else
    {
        // 戦闘外はターゲットを忘れる
        CurrentTarget = nullptr;
    }


    UpdateAnimationState();
}

void AUnit::StartDrag(const FVector& MouseWorld)
{
    if (!bCanDrag || bIsDragging) return;

    bIsDragging = true;
    UnitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    OriginalLocation = GetActorLocation();

    // ★ ドラッグ開始時にいたタイルを覚える
    DragStartTile = CurrentTile;

    // 一旦タイルを空ける
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
        // 正常にタイルに落とせたとき
        OwningBoardManager->MoveUnitToTile(this, Tile);
    }
    else
    {
        // ★ タイルの外 → 位置もタイル情報も元に戻す
        SetActorLocation(OriginalLocation);

        if (DragStartTile)
        {
            CurrentTile = DragStartTile;
            DragStartTile->bIsOccupied = true;
            DragStartTile->OccupiedUnit = this;
        }
    }

    DragStartTile = nullptr;
}


void AUnit::CheckForTarget(float DeltaTime)
{
    if (bIsDead) return;

    // --- 1. 既存ターゲットのチェック ---
    if (CurrentTarget)
    {
        // 死んでたらターゲット解除
        if (CurrentTarget->bIsDead || CurrentTarget->HP <= 0.f)
        {
            CurrentTarget = nullptr;
            bIsAttacking = false;
        }
    }

    // --- 2. ターゲットがいなければ探す ---
    if (!CurrentTarget)
    {
        AUnit* ClosestEnemy = nullptr;
        float ClosestDist = FLT_MAX;

        for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
        {
            AUnit* Other = *It;
            if (Other == this) continue;
            if (Other->Team == Team) continue;
            if (Other->bIsDead || Other->HP <= 0.f) continue;

            float Dist = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
            if (Dist < ClosestDist)
            {
                ClosestDist = Dist;
                ClosestEnemy = Other;
            }
        }

        CurrentTarget = ClosestEnemy;
    }

    // 敵いなければ何もしない
    if (!CurrentTarget) return;

    // --- 3. 射程内なら攻撃、射程外なら移動 ---
    const float DistToTarget = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

    if (DistToTarget <= Range)
    {
        UpdateFacing(DeltaTime);

        TimeSinceLastAttack += DeltaTime;
        if (TimeSinceLastAttack >= AttackInterval)
        {
            TimeSinceLastAttack = 0.f;
            AttackTarget(CurrentTarget);
        }
    }
    else
    {
        bIsAttacking = false;

        FVector NewLoc = FMath::VInterpConstantTo(
            GetActorLocation(),
            CurrentTarget->GetActorLocation(),
            DeltaTime,
            MoveSpeed
        );
        NewLoc.Z = GetActorLocation().Z; // 高さは固定
        SetActorLocation(NewLoc);
    }
}


void AUnit::AttackTarget(AUnit* Target)
{
    if (!Target || Target->bIsDead) return;

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
    if (bIsDead) return;

    float FinalDamage = FMath::Max(1.f, DamageAmount - Defense);
    HP -= FinalDamage;

    ShowDamagePopup(FinalDamage, /*bIsMagicDamage=*/false);
    bLastHitWasCritical = false; // 使い終わったらリセット

    if (HP <= 0.f && !bIsDead)
    {
        OnDeath();
    }
}

void AUnit::TakeMagicDamage(float DamageAmount)
{
    if (bIsDead) return;

    float FinalDamage = FMath::Max(1.f, DamageAmount - MagicDefense);
    HP -= FinalDamage;

    // ★ APダメージ → 青ポップアップ
    ShowDamagePopup(FinalDamage, /*bIsMagicDamage=*/true);


    UE_LOG(LogTemp, Warning,
        TEXT("%s Takes Magic Damage: %.1f  HP=%.1f  Percent=%.2f"),
        *GetName(), FinalDamage, HP, GetHPPercent());

    if (HP <= 0.f && !bIsDead)
    {
        OnDeath();
    }
}



void AUnit::OnDeath()
{
    if (bIsDead) return; // 二重呼び出し防止

    bIsDead = true;
    bIsAttacking = false;
    bIsMoving = false;
    bIsDragging = false;
    bCanDrag = false;

    // ★ タイル処理をチームで分ける
    if (CurrentTile)
    {
        if (Team == EUnitTeam::Enemy)
        {
            // 敵は死んだらその場のマスをすぐ空けてOK
            CurrentTile->bIsOccupied = false;
            CurrentTile->OccupiedUnit = nullptr;
        }

        // プレイヤーユニットは CurrentTile を残す
        // → MakeSaveData で正しい TileIndex を保存できるようにする
    }

    // ターゲット解除
    CurrentTarget = nullptr;

    // これ以上攻撃対象にならないようにコリジョンを切る
    if (UnitMesh)
    {
        UnitMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (OwningBoardManager && OwningBoardManager->ItemUnit == this)
    {
        OwningBoardManager->SetItemTargetUnit(nullptr);
    }


    // ★ Destroy はここではしない（敵は OnDeathFinished で Destroy、プレイヤーは残す）
}


void AUnit::OnDeathFinished()
{
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
    if (Item.EffectType == "Defense")Defense += Item.EffectValue;
    if (Item.EffectType == "MagicPower")MagicPower += Item.EffectValue;
    if (Item.EffectType == "MagicDefense")MagicDefense += Item.EffectValue;
    if (Item.EffectType == "Range")Range += Item.EffectValue;
    if (Item.EffectType == "MoveSpeed")MoveSpeed += Item.EffectValue;
    if (Item.EffectType == "CritChance")     CritChance += Item.EffectValue;
    if (Item.EffectType == "CritMultiplier") CritMultiplier += Item.EffectValue;

    if (Item.EffectType == "AttackInterval")
    {
        // EffectValue に -0.2 とか入れると「0.2秒短くなる」
        AttackInterval = FMath::Max(0.1f, AttackInterval + Item.EffectValue);
    }

}

void AUnit::ReapplayAllItemEffects()
{
    HP = BaseHP;
    Attack = BaseAttack;
    Defense = BaseDefense;
    MagicPower = BaseMagicPower;
    MagicDefense = BaseMagicDefense;
    Range = BaseRange;
    MoveSpeed = BaseMoveSpeed;
    CritChance = CritChance;
    CritMultiplier = CritMultiplier;

    AttackInterval = BaseAttackInterval;

    for (auto& Item : EquipedItems)
    {
        ApplyItemEffect(Item);
    }
}

void AUnit::RemoveItems()
{
    // 装備アイテム配列を空っぽに
    EquipedItems.Empty();

    // ステータスを基礎値＋アイテム無しの状態に戻す
    ReapplayAllItemEffects();

    UE_LOG(LogTemp, Warning,
        TEXT("[Unit] RemoveAllItems called on %s"), *GetName());
}

FUnitSaveData AUnit::MakeSaveData()
{
    FUnitSaveData Data;
    Data.UnitID = UnitID;
    Data.BaseHP = BaseHP;
    Data.BaseAttack = BaseAttack;
    Data.BaseDefense = BaseDefense;
    Data.BaseMagicPower = BaseMagicPower;
    Data.BaseMagicDefense = BaseMagicDefense;
    Data.BaseRange = BaseRange;
    Data.BaseMoveSpeed = BaseMoveSpeed;

    Data.CritChance = CritChance;
    Data.CritMultiplier = CritMultiplier;

    Data.BaseAttackInterval = AttackInterval;

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
    BaseRange = Data.BaseRange;
    BaseMoveSpeed = Data.BaseMoveSpeed;

    MaxHP = BaseHP;

    BaseAttackInterval = Data.BaseAttackInterval;

    CritChance = Data.CritChance;
    CritMultiplier = Data.CritMultiplier;

    EquipedItems = Data.EquippedItems;

    //一旦素の値を反映
    HP = BaseHP;
    Attack = BaseAttack;
    Defense = BaseDefense;
    MagicPower = BaseMagicPower;
    MagicDefense = BaseMagicDefense;
    Range = BaseRange;
    MoveSpeed = BaseMoveSpeed;
    CritChance = CritChance;
    CritMultiplier = CritMultiplier;

    AttackInterval = BaseAttackInterval;

    bIsDead = false;
    bIsAttacking = false;
    bIsMoving = false;
    bIsDragging = false;
    bCanDrag = true;

    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);

    if (UnitMesh)
    {
        UnitMesh->SetVisibility(true, true);
        UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

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

    // ① すでに出ているホバーUIを全部消す
    {
        TArray<UUserWidget*> Existing;
        UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
            this,
            Existing,
            UUnitHoverInfoWidget::StaticClass(),
            false
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
        HoverWidget->OwnerUnit = this;

        HoverWidget->SetUnitInfo(
            UnitID,
            HP,
            Attack,
            Defense,
            MagicPower,
            MagicDefense,
            Range,
            MoveSpeed,
            CritChance,
            CritMultiplier,
            AttackInterval,
            EquipedItems
        );

        HoverWidget->AddToViewport();

        // ★ ここがポイント：後ろのクリックをブロックしない
        HoverWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        // ※ 子ウィジェットはヒットテストされるけど、
        //   ホバー全体としては後ろのボード / ベンチにクリックが通る
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
    UE_LOG(LogTemp, Warning,
        TEXT("[Unit] OnUnitClicked %s Button=%s"),
        *GetName(), *ButtonPressed.ToString());

    // 右クリック：このユニットをアイテム装備対象にする ＋ 情報ホバー表示
    if (ButtonPressed == EKeys::LeftMouseButton)
    {
        if (OwningBoardManager)
        {
            OwningBoardManager->SetItemTargetUnit(this);
        }

        ShowUnitInfo();
    }
}

void AUnit::UpdateFacing(float DeltaTime)
{
    if (!bFaceTarget || bIsDead || !CurrentTarget) return;

    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = CurrentTarget->GetActorLocation();
    FVector ToTarget = TargetLoc - MyLoc;
    ToTarget.Z = 0.f;

    if (ToTarget.IsNearlyZero()) return;

    FRotator TargetRot = ToTarget.Rotation();
    TargetRot.Yaw += FacingYawOffset;
    TargetRot.Pitch = 0.f;
    TargetRot.Roll = 0.f;

    // ☆ ここがポイント：Actor じゃなくて Mesh を回す
    FRotator NewRot = FMath::RInterpTo(
        UnitMesh->GetComponentRotation(),
        TargetRot,
        DeltaTime,
        RotationInterpSpeed
    );

    UnitMesh->SetWorldRotation(NewRot);
}

void AUnit::RefreshHoverInfo()
{
    // 今ホバーが出ているなら、中身だけ更新
    if (HoverWidget)
    {
        HoverWidget->SetUnitInfo(
            UnitID,
            HP,
            Attack,
            Defense,
            MagicPower,
            MagicDefense,
            Range,
            MoveSpeed,
            CritChance,
            CritMultiplier,
            AttackInterval,
            EquipedItems
        );
    }
}

float AUnit::GetHPPercent() const
{
    if (MaxHP <= 0.f)
    {
        return 0.f;
    }

    // 念のため 0～1 にクランプしておく
    const float Raw = HP / MaxHP;
    return FMath::Clamp(Raw, 0.f, 1.f);
}

void AUnit::ShowDamagePopup(float DamageAmount, bool bIsMagicDamage)
{
    if (!DamagePopupWidgetClass) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    UDamagePopupWidget* Popup =
        CreateWidget<UDamagePopupWidget>(PC, DamagePopupWidgetClass);
    if (!Popup) return;

    Popup->AddToViewport();

    // ★ ここでローカル変数を作る
    bool bIsCritical = bLastHitWasCritical;

    Popup->SetupDamage(DamageAmount, bIsMagicDamage, bIsCritical);

    FVector WorldLoc = GetActorLocation() + FVector(0.f, 0.f, 120.f);
    FVector2D ScreenPos;
    if (PC->ProjectWorldLocationToScreen(WorldLoc, ScreenPos))
    {
        const float MaxOffsetY = 6.f;
        float OffsetY = FMath::RandRange(-MaxOffsetY, MaxOffsetY);
        FVector2D JitteredPos = ScreenPos + FVector2D(0.f, OffsetY);
        Popup->SetPositionInViewport(JitteredPos, true);
    }

    // 使い終わったらリセットしておく
    bLastHitWasCritical = false;
}

void AUnit::ShowHealPopup(float HealAmount)
{
    if (!DamagePopupWidgetClass) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    UDamagePopupWidget* Popup =
        CreateWidget<UDamagePopupWidget>(PC, DamagePopupWidgetClass);
    if (!Popup) return;

    Popup->AddToViewport();
    Popup->SetupHeal(HealAmount);   // ← さっき追加したやつ

    FVector WorldLoc = GetActorLocation() + FVector(0.f, 0.f, 120.f);
    FVector2D ScreenPos;
    if (PC->ProjectWorldLocationToScreen(WorldLoc, ScreenPos))
    {
        const float MaxOffsetY = 6.f;
        float OffsetY = FMath::RandRange(-MaxOffsetY, MaxOffsetY);

        FVector2D JitteredPos = ScreenPos + FVector2D(0.f, OffsetY);
        Popup->SetPositionInViewport(JitteredPos, true);

    }
}



float AUnit::CalcPhysicalDamageWithCrit(float BaseDamage, bool& bOutIsCritical)
{
    bOutIsCritical = false;

    if (CritChance > 0.f && CritMultiplier > 1.f)
    {
        float Roll = FMath::FRand(); // 0.0～1.0
        if (Roll < CritChance)
        {
            bOutIsCritical = true;
            return BaseDamage * CritMultiplier;
        }
    }

    return BaseDamage;
}
