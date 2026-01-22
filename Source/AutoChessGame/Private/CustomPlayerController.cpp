#include "CustomPlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Unit.h"
#include "BoardManager.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "Components/TextBlock.h"
#include "Engine/GameViewportClient.h"
#include "PlayerManager.h"
#include "LevelUpRewardWidget.h"


ACustomPlayerController::ACustomPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    SelectedUnit = nullptr;
    bIsDragging = false;
    LastHighlightedTile = nullptr;

    PrimaryActorTick.bCanEverTick = true;
}

FORCEINLINE bool ACustomPlayerController::IsLevelUpLockingInput() const
{
    return bIsLevelUpChoosing || bIsLevelUpAnimating;
}


void ACustomPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction("LeftClick", IE_Pressed, this, &ACustomPlayerController::OnLeftMouseDown);
        InputComponent->BindAction("LeftClick", IE_Released, this, &ACustomPlayerController::OnLeftMouseUp);
        InputComponent->BindAction("RightClick", IE_Pressed, this, &ACustomPlayerController::OnRightClick);
        InputComponent->BindAction("Pause", IE_Pressed, this, &ACustomPlayerController::TogglePauseMenu);
    
        InputComponent->BindAxis("MouseWheelAxis", this, &ACustomPlayerController::OnZoomAxis);
    }
    UE_LOG(LogTemp, Warning, TEXT("[PC] SetupInputComponent done"));
}

void ACustomPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UpdateCameraZoom(DeltaSeconds);

    if (bIsDragging && SelectedUnit)
    {
        float MouseX, MouseY;
        if (GetMousePosition(MouseX, MouseY))
        {
            FVector WorldOrigin, WorldDir;
            DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDir);

            FVector MouseWorldPoint = WorldOrigin + WorldDir * 500.f;
            SelectedUnit->UpdateDrag(MouseWorldPoint);
        }

        // タイルハイライト
        FHitResult Hit;
        if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
        {
            ATile* HoveredTile = Cast<ATile>(Hit.GetActor());

            if (LastHighlightedTile && LastHighlightedTile != HoveredTile)
            {
                LastHighlightedTile->SetTileHighlight(false);
            }

            if (HoveredTile && HoveredTile->bIsPlayerTile && !HoveredTile->bIsOccupied)
            {
                HoveredTile->SetTileHighlight(true);
                LastHighlightedTile = HoveredTile;
            }
            else if (LastHighlightedTile)
            {
                LastHighlightedTile->SetTileHighlight(false);
                LastHighlightedTile = nullptr;
            }
        }
        else if (LastHighlightedTile)
        {
            LastHighlightedTile->SetTileHighlight(false);
            LastHighlightedTile = nullptr;
        }
    }
}

void ACustomPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // まずレベルに置いた CameraActor を探す（1個目を使用）
    if (!BoradCameraActor)
    {
        TArray<AActor*> Cameras;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), Cameras);
        if (Cameras.Num() > 0)
        {
            BoradCameraActor = Cameras[0];
        }
    }

    if (ACameraActor* CamActor = Cast<ACameraActor>(BoradCameraActor))
    {
        if (UCameraComponent* Cam = CamActor->GetCameraComponent())
        {
            Cam->AspectRatio = 16.0f / 9.0f;
            Cam->bConstrainAspectRatio = true;
        }

        FViewTargetTransitionParams Params;
        Params.BlendTime = 0.0f;
        SetViewTarget(CamActor, Params);

        bShowMouseCursor = true;
        DefaultMouseCursor = EMouseCursor::Default;

        // ===== ズーム初期化 =====
        // Pivotは BoardManager の位置が理想（見つからなければカメラ前方に仮置き）
        if (ABoardManager* BM = Cast<ABoardManager>(UGameplayStatics::GetActorOfClass(this, ABoardManager::StaticClass())))
        {
            Pivot = BM->GetActorLocation();
        }
        else
        {
            Pivot = CamActor->GetActorLocation() + CamActor->GetActorForwardVector() * 2000.f;
        }

        const FVector CamLoc = CamActor->GetActorLocation();
        CameraDir = (CamLoc - Pivot).GetSafeNormal();

        TargetDistance = FVector::Distance(CamLoc, Pivot);
        TargetDistance = FMath::Clamp(TargetDistance, MinDistance, MaxDistance);
    
        InitialPivot = Pivot;
        InitialCameraDir = CameraDir;
        InitialDistance = TargetDistance;

        TargetDistance = MaxDistance;

        CamActor->SetActorLocation(Pivot + CameraDir * TargetDistance);
    }

    // ===== 以下、あなたの元の処理はそのまま =====

    const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);
    UE_LOG(LogTemp, Warning, TEXT("[PC] Level=%s"), *LevelName);

    if (LevelName == TEXT("L_Title"))
    {
        EnterTitleMode();
        return;
    }
    else
    {
        EnterGameMode();
    }

    if (OverlayClass)
    {
        OverlayWidget = CreateWidget<UUserWidget>(this, OverlayClass);
        if (OverlayWidget)
        {
            OverlayWidget->AddToViewport(999);
        }
    }
    LockInputForIntro(true);

    CallOverlayEventByName(TEXT("PlayGameStart"));
}

void ACustomPlayerController::RequestBattleStartUI(ABoardManager* BoardManager)
{
    if (bIsLevelUpChoosing)return;

    PendingBoardManager = BoardManager;

    if (!OverlayWidget && OverlayClass)
    {
        OverlayWidget = CreateWidget<UUserWidget>(this, OverlayClass);
        if (OverlayWidget)
        {
            OverlayWidget->AddToViewport(999);
        }
    }

    // ★バトル開始演出中：操作は止めるが、カーソルは出す
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);

    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    FInputModeUIOnly Mode;
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);

    CallOverlayEventByName(TEXT("PlayBattleStart"));
}

void ACustomPlayerController::OnGameStartUIFinished()
{
    UE_LOG(LogTemp, Warning, TEXT("[Intro] OnGameStartUIFinished called"));

    // Introで止めた入力を戻す
    SetIgnoreMoveInput(false);
    SetIgnoreLookInput(false);

    // Viewportのキャプチャを解除（←ここが効く）
    if (UGameViewportClient* Viewport = GetWorld() ? GetWorld()->GetGameViewport() : nullptr)
    {
        Viewport->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
        Viewport->SetMouseLockMode(EMouseLockMode::DoNotLock);
    }

    // InputMode をゲーム＋UIに戻す
    FInputModeGameAndUI Mode;
    Mode.SetHideCursorDuringCapture(false);
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);

    // マウスを確実にON（最後にもう一回）
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    bShowMouseCursor = true;

    // Overlay消す
    if (OverlayWidget)
    {
        OverlayWidget->RemoveFromParent();
        OverlayWidget = nullptr;
    }

    EnterGameMode();
}

void ACustomPlayerController::OnBattleStartUIFinished()
{
    if (OverlayWidget)
    {
        OverlayWidget->RemoveFromParent();
        OverlayWidget = nullptr;
    }

    // ★入力をゲームへ戻す（ここが安定）
    EnterGameMode();

    if (PendingBoardManager.IsValid())
    {
        PendingBoardManager->StartBattlePhase();
    }
    PendingBoardManager.Reset();
}


void ACustomPlayerController::LockInputForIntro(bool bLock)
{
    SetIgnoreMoveInput(bLock);
    SetIgnoreLookInput(bLock);

    if (bLock)
    {
        bShowMouseCursor = false;
        FInputModeGameOnly Mode;
        SetInputMode(Mode);
    }
    else
    {
        bShowMouseCursor = true; // あなたのゲームがマウス操作なら true 推奨
        FInputModeGameOnly Mode;
        SetInputMode(Mode);
    }
}

void ACustomPlayerController::CallOverlayEventByName(FName EventName)
{
    if (!OverlayWidget) return;

    if (UFunction* Func = OverlayWidget->FindFunction(EventName))
    {
        OverlayWidget->ProcessEvent(Func, nullptr);
    }
}

void ACustomPlayerController::OnLeftMouseDown()
{
    if (IsLevelUpLockingInput()) return;

    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (Hit.bBlockingHit)
    {
        if (AUnit* Unit = Cast<AUnit>(Hit.GetActor()))
        {
            if (!Unit->bCanDrag)return;

            SelectedUnit = Unit;
            bIsDragging = true;
            SelectedUnit->OriginalLocation = SelectedUnit->GetActorLocation();
            SelectedUnit->StartDrag(Hit.ImpactPoint);
        }
    }
}

void ACustomPlayerController::OnLeftMouseUp()
{
    if (IsLevelUpLockingInput()) return;

    // ★ そもそも掴んでない/消えた/無効なら何もしない
    if (!IsValid(SelectedUnit))
    {
        bIsDragging = false;
        SelectedUnit = nullptr;
        return;
    }

    bIsDragging = false;

    FHitResult Hit;
    const bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (bHit && Hit.bBlockingHit)
    {
        ATile* Tile = Cast<ATile>(Hit.GetActor());
        if (IsValid(Tile) && Tile->bIsPlayerTile)
        {
            if (IsValid(SelectedUnit->OwningBoardManager))
            {
                SelectedUnit->OwningBoardManager->MoveUnitToTile(SelectedUnit, Tile);
            }
            else
            {
                SelectedUnit->SetActorLocation(SelectedUnit->OriginalLocation);
            }
        }
        else
        {
            SelectedUnit->SetActorLocation(SelectedUnit->OriginalLocation);
        }
    }
    else
    {
        SelectedUnit->SetActorLocation(SelectedUnit->OriginalLocation);
    }

    // ★ EndDrag 内でDestroyとか触るなら、順序も安全寄りに
    SelectedUnit->EndDrag();
    SelectedUnit = nullptr;
}

void ACustomPlayerController::OnRightClick()
{
    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (!Hit.bBlockingHit)
    {
        CloseAllUnitInfoWidgets();
        return;
    }

    if (AUnit* Unit = Cast<AUnit>(Hit.GetActor()))
    {
        // ① ホバー表示（今まで通り）
        Unit->ShowUnitInfo();

        // ② BoardManager を取得
        ABoardManager* BM = Cast<ABoardManager>(
            UGameplayStatics::GetActorOfClass(this, ABoardManager::StaticClass()));

        if (BM)
        {
            BM->ItemUnit = Unit;
            UE_LOG(LogTemp, Warning,
                TEXT("[PC] RightClick: Set ItemUnit = %s on %s"),
                *Unit->GetName(),
                *BM->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[PC] RightClick: BoardManager not found"));
        }

        return;
    }

    // ユニット以外を右クリックしたら全部閉じる
    CloseAllUnitInfoWidgets();
}
void ACustomPlayerController::OnLeftClick()
{
    
}

void ACustomPlayerController::EnterTitleMode()
{
    // タイトルUI生成
    if (TitleWidgetClass && !TitleWidgetInstance)
    {
        TitleWidgetInstance = CreateWidget<UUserWidget>(this, TitleWidgetClass);
        if (TitleWidgetInstance)
        {
            TitleWidgetInstance->AddToViewport(100);
        }
    }

    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    FInputModeUIOnly Mode;
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
}

void ACustomPlayerController::EnterGameMode()
{
    // タイトルUIが残ってたら消す（L_Title→L_Game の事故対策）
    if (TitleWidgetInstance)
    {
        TitleWidgetInstance->RemoveFromParent();
        TitleWidgetInstance = nullptr;
    }

    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    // ドラッグやクリックでワールド入力を通す
    FInputModeGameAndUI Mode;
    Mode.SetHideCursorDuringCapture(false);
    // Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 好みで
    SetInputMode(Mode);
}


void ACustomPlayerController::CloseAllUnitInfoWidgets()
{
    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Unit = *It;
        Unit->HideUnitInfo();
    }
}

void ACustomPlayerController::TogglePauseMenu()
{
    // すでに出てるなら閉じる（=Resume）
    if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
    {
        ResumeGame();
        return;
    }

    // 生成
    if (!PauseMenuClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PC] PauseMenuClass is NULL"));
        return;
    }

    PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
    if (!PauseMenuInstance) return;

    PauseMenuInstance->AddToViewport(200);

    // Pause
    SetPause(true);

    // 入力：UI操作できるように
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    FInputModeUIOnly Mode;
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
}

void ACustomPlayerController::ResumeGame()
{
    if (PauseMenuInstance)
    {
        PauseMenuInstance->RemoveFromParent();
        PauseMenuInstance = nullptr;
    }

    SetPause(false);

    // 入力：ゲームに戻す（ドラッグできる状態）
    FInputModeGameAndUI Mode;
    Mode.SetHideCursorDuringCapture(false);
    SetInputMode(Mode);

    bShowMouseCursor = true; // ドラッグ前提ならtrueのままでOK
}

void ACustomPlayerController::ReturnToTitle()
{
    // 先に戻す（重要：Pauseしたまま移動すると詰むことがある）
    ResumeGame();

    UGameplayStatics::OpenLevel(this, FName(TEXT("L_Title")));
}

void ACustomPlayerController::ShowEndMenu(bool bGameClear)
{
    if (!EndMenuClass) return;

    if (!EndMenuInstance)
    {
        EndMenuInstance = CreateWidget<UUserWidget>(this, EndMenuClass);
    }
    if (EndMenuInstance && !EndMenuInstance->IsInViewport())
    {
        EndMenuInstance->AddToViewport(300);
    }

    // 入力をUIへ
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    FInputModeUIOnly Mode;
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);

    // ゲーム止める（EndはPauseでOK）
    SetPause(true);

    if (EndMenuInstance)
    {
        if (UTextBlock* Title = Cast<UTextBlock>(EndMenuInstance->GetWidgetFromName(TEXT("TitleText"))))
        {
            Title->SetText(FText::FromString(bGameClear ? TEXT("GAME CLEAR") : TEXT("GAME OVER")));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[EndMenu] TitleText not found. Check widget name in WBP_EndMenu."));
        }
    }
    // bGameClear をWBPに渡したいなら、WBP側で GetOwningPlayer→Cast→何か参照でもOK
}

void ACustomPlayerController::HideEndMenu()
{
    if (EndMenuInstance)
    {
        EndMenuInstance->RemoveFromParent();
        EndMenuInstance = nullptr;
    }

    SetPause(false);

    FInputModeGameAndUI Mode;
    Mode.SetHideCursorDuringCapture(false);
    SetInputMode(Mode);

    bShowMouseCursor = true;
}

void ACustomPlayerController::ShowLevelUpRewardUI(APlayerManager* PM, const TArray<FName>& Candidates)
{
    bIsLevelUpAnimating = false;      // ★保険：演出終わった扱いにする
    bEnableClickEvents = true;        // ★リワード選択できるように戻す
    bEnableMouseOverEvents = true;

    if (bIsLevelUpChoosing) return;
    bIsLevelUpChoosing = true;
    

    if (!PM || Candidates.Num() == 0) { bIsLevelUpChoosing = false; return; }

    // 既に出てたら消す
    if (LevelUpRewardWidget)
    {
        LevelUpRewardWidget->RemoveFromParent();
        LevelUpRewardWidget = nullptr;
    }

    // ★LevelUpRewardClass は「あなたが作ってある LevelUpReward（ULevelUpRewardWidget）BP」を指定
    ULevelUpRewardWidget* W = LevelUpRewardClass
        ? CreateWidget<ULevelUpRewardWidget>(this, LevelUpRewardClass)
        : nullptr;

    UE_LOG(LogTemp, Warning, TEXT("[LevelUp] LevelUpRewardClass=%s"), *GetNameSafe(LevelUpRewardClass));
    UE_LOG(LogTemp, Warning, TEXT("[LevelUp] OverlayWidget=%s"), *GetNameSafe(OverlayWidget));
    UE_LOG(LogTemp, Warning, TEXT("[LevelUp] Before AddToViewport W=%s"), *GetNameSafe(W));

    if (!W) { bIsLevelUpChoosing = false; return; }

    LevelUpRewardWidget = W;
    W->OwnerPlayerManager = PM;
    W->SetupChoices(Candidates);
    W->AddToViewport(9999);

    // 入力をUIへ
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);

    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    FInputModeUIOnly Mode;
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
}

void ACustomPlayerController::OnLevelUpRewardPicked(FName UnitID)
{
    UE_LOG(LogTemp, Warning, TEXT("[LevelUp] Picked: %s"), *UnitID.ToString());

    // BoardManagerへ反映（あなたは SpawnRewardUnit が既にある）
    if (ABoardManager* BM = Cast<ABoardManager>(UGameplayStatics::GetActorOfClass(this, ABoardManager::StaticClass())))
    {
        BM->SpawnRewardUnit(UnitID);
    }

    // UI閉じる
    if (LevelUpRewardWidget)
    {
        LevelUpRewardWidget->RemoveFromParent();
        LevelUpRewardWidget = nullptr;
    }

    // 入力をゲームへ戻す（あなたの関数が安定）
    EnterGameMode();

    bIsLevelUpChoosing = false;
}

void ACustomPlayerController::EndLevelUpRewardUI()
{
    if (LevelUpRewardWidget)
    {
        LevelUpRewardWidget->RemoveFromParent();
        LevelUpRewardWidget = nullptr;
    }

    EnterGameMode();
    bIsLevelUpChoosing = false;
}

void ACustomPlayerController::PlayLevelUpUI()
{
    if (bIsLevelUpChoosing || bIsLevelUpAnimating) return;

    bIsLevelUpAnimating = true;

    if (!OverlayWidget && OverlayClass)
    {
        OverlayWidget = CreateWidget<UUserWidget>(this, OverlayClass);
        if (OverlayWidget) OverlayWidget->AddToViewport(999);
    }

    // ★ 演出中：ワールドクリックを完全停止
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);

    bShowMouseCursor = true;

    bEnableClickEvents = false;       // ★ここをfalseのまま
    bEnableMouseOverEvents = false;   // ★ホバーも止めるならfalse

    FInputModeUIOnly Mode;
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);

    CallOverlayEventByName(TEXT("PlayLevelUp"));
}


void ACustomPlayerController::OnZoomAxis(float AxisValue)
{
    if (IsPaused() || bIsLevelUpChoosing) return;
    if (FMath::IsNearlyZero(AxisValue)) return;
    if (!BoradCameraActor) return;

    const bool bZoomIn = (AxisValue > 0.f);   // 環境で逆なら < にする
    const bool bZoomOut = (AxisValue < 0.f);

    if (bZoomIn)
    {
        FHitResult Hit;
        if (GetHitResultUnderCursor(ECC_Visibility, false, Hit) && Hit.bBlockingHit)
        {
            Pivot = Hit.ImpactPoint;
            CameraDir = (BoradCameraActor->GetActorLocation() - Pivot).GetSafeNormal();
        }
    }
    else if (bZoomOut)
    {
        // ★戻す時は初期位置基準に戻す
        Pivot = InitialPivot;
        CameraDir = InitialCameraDir;
        // 必要なら距離も初期に寄せたい場合はここで
        // TargetDistance = FMath::Max(TargetDistance, InitialDistance);
    }

    TargetDistance = FMath::Clamp(
        TargetDistance - AxisValue * ZoomStep,
        MinDistance,
        MaxDistance
    );
}

void ACustomPlayerController::UpdateCameraZoom(float DeltaSeconds)
{
    if (!BoradCameraActor) return;

    AActor* CamActor = BoradCameraActor;
    const FVector CurrentLoc = CamActor->GetActorLocation();
    const FVector DesiredLoc = Pivot + CameraDir * TargetDistance;

    const FVector NewLoc = FMath::VInterpTo(CurrentLoc, DesiredLoc, DeltaSeconds, ZoomInterSpeed);
    CamActor->SetActorLocation(NewLoc);
}