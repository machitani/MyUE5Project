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
#include "Components/TextBlock.h"

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

void ACustomPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction("LeftClick", IE_Pressed, this, &ACustomPlayerController::OnLeftMouseDown);
        InputComponent->BindAction("LeftClick", IE_Released, this, &ACustomPlayerController::OnLeftMouseUp);
        InputComponent->BindAction("RightClick", IE_Pressed, this, &ACustomPlayerController::OnRightClick);
        InputComponent->BindAction("Pause", IE_Pressed, this, &ACustomPlayerController::TogglePauseMenu);
    }
    UE_LOG(LogTemp, Warning, TEXT("[PC] SetupInputComponent done"));
}

void ACustomPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

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

    ACameraActor* FixedCamera = nullptr;

    TArray<AActor*> Cameras;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), Cameras);

    if (Cameras.Num() > 0)
    {
        FixedCamera = Cast<ACameraActor>(Cameras[0]); // 1個目を使う（1個しか置かない前提）
    }

    if (FixedCamera)
    {

        if (UCameraComponent* Cam = FixedCamera->GetCameraComponent())
        {
            Cam->AspectRatio = 16.0f / 9.0f;      // 1.77778...
            Cam->bConstrainAspectRatio = true;    // アスペクト比を固定
        }

        FViewTargetTransitionParams Params;
        Params.BlendTime = 0.0f; // フェード無しで即切り替え

        SetViewTarget(FixedCamera, Params);

        // マウス操作用（ボードゲームならマウスカーソル出しておくと便利）
        bShowMouseCursor = true;
        DefaultMouseCursor = EMouseCursor::Default;
    }

   

    const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);
    UE_LOG(LogTemp, Warning, TEXT("[PC] Level=%s"), *LevelName);

    if (LevelName == TEXT("L_Title"))
    {
        EnterTitleMode();
    }
    else
    {
        EnterGameMode();
    }
}

void ACustomPlayerController::OnLeftMouseDown()
{
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
    if (!bIsDragging || !SelectedUnit)
        return;

    bIsDragging = false;

    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (Hit.bBlockingHit)
    {
        ATile* Tile = Cast<ATile>(Hit.GetActor());
        if (Tile && Tile->bIsPlayerTile)
        {
            // ★ Tile が埋まってるかどうかの判定は MoveUnitToTile 側でやる
            if (SelectedUnit->OwningBoardManager)
            {
                SelectedUnit->OwningBoardManager->MoveUnitToTile(SelectedUnit, Tile);
            }
            else
            {
                // BoardManager が無いなら元の位置に戻す
                SelectedUnit->SetActorLocation(SelectedUnit->OriginalLocation);
            }
        }
        else
        {
            // タイル以外 or 敵タイル → 元の位置に戻す
            SelectedUnit->SetActorLocation(SelectedUnit->OriginalLocation);
        }
    }
    else
    {
        SelectedUnit->SetActorLocation(SelectedUnit->OriginalLocation);
    }

    // 位置＆タイルはもう決まってるので、EndDrag は「ドラッグ終了の後始末」だけに使う
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