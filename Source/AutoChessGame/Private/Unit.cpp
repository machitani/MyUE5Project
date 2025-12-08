#include "Unit.h"
#include "Tile.h"
#include "BoardManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BoardManager.h"
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

    UE_LOG(LogTemp, Warning, TEXT("[Unit] Constructor: %s  binding OnClicked"), *GetName());

    UnitMesh->OnClicked.AddDynamic(this, &AUnit::OnUnitClicked);

    bIsDragging = false;
    bCanDrag = true;

    PrimaryActorTick.bCanEverTick = true;

    HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
    HPBarWidget->SetupAttachment(RootComponent);

    // World空間で3Dオブジェクトとして出す（まずは確実に見える形）
    HPBarWidget->SetWidgetSpace(EWidgetSpace::World);

    // HPバーの表示位置（頭の上）
    HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
    HPBarWidget->SetDrawSize(FVector2D(150.f, 15.f));

    // コリジョン不要
    HPBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // ★ここでWidgetClassをセットするか、BP上で設定する
    // C++でやる場合（パスは自分のに合わせて変える）
    static ConstructorHelpers::FClassFinder<UUserWidget> HPBarClass(TEXT("/Game/UI/WBP_UnitHP"));
    if (HPBarClass.Succeeded())
    {
        HPBarWidget->SetWidgetClass(HPBarClass.Class);
    }
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

    LastLocation = GetActorLocation();

    if (HPBarWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Unit] %s HPBarWidget is valid"), *GetName());

        if (HPBarWidget->GetWidgetClass() == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("[Unit] %s HPBarWidget has NO WidgetClass"), *GetName());
        }
        else
        {
            UUserWidget* W = HPBarWidget->GetUserWidgetObject();
            if (W)
            {
                UE_LOG(LogTemp, Warning, TEXT("[Unit] %s HPBar widget created"), *GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[Unit] %s HPBar widget NOT created"), *GetName());
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
    //HP = FMath::Clamp(HP, 0.f, MaxHP);

    UE_LOG(LogTemp, Warning, TEXT("%s Takes Physical Damage: %.1f"), *GetName(), FinalDamage);

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
    //HP = FMath::Clamp(HP, 0.f, MaxHP);

    UE_LOG(LogTemp, Warning, TEXT("%s Takes Magic Damage: %.1f"), *GetName(), FinalDamage);

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

    EquipedItems = Data.EquippedItems;

    //一旦素の値を反映
    HP = BaseHP;
    Attack = BaseAttack;
    Defense = BaseDefense;
    MagicPower = BaseMagicPower;
    MagicDefense = BaseMagicDefense;
    Range = BaseRange;
    MoveSpeed = BaseMoveSpeed;

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
    if (!bFaceTarget) return;
    if (bIsDead)     return;
    if (!CurrentTarget) return;

    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = CurrentTarget->GetActorLocation();

    FVector ToTarget = TargetLoc - MyLoc;
    ToTarget.Z = 0.f;              // 2D 平面に投影

    if (ToTarget.IsNearlyZero())
    {
        return;
    }

    // ターゲット方向の角度
    FRotator TargetRot = ToTarget.Rotation();

    // （必要ならオフセットで微調整）
    TargetRot.Yaw += FacingYawOffset;   // 普段は 0 でOK

    TargetRot.Pitch = 0.f;
    TargetRot.Roll = 0.f;

    // なめらかに向きを合わせる
    FRotator NewRot = FMath::RInterpTo(
        GetActorRotation(),
        TargetRot,
        DeltaTime,
        RotationInterpSpeed   // 1015 くらいが分かりやすい
    );

    SetActorRotation(NewRot);
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
    return HP / MaxHP;
}
