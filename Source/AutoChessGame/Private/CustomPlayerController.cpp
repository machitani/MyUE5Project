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
#include "Kismet/KismetSystemLibrary.h"

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
        if (Tile && Tile->bIsPlayerTile && !Tile->bIsOccupied)
        {
            FVector TargetLoc = Tile->GetActorLocation() + FVector(0, 0, 150.f);
            SelectedUnit->SetActorLocation(TargetLoc);

            Tile->bIsOccupied = true;
            Tile->OccupiedUnit = SelectedUnit;
            SelectedUnit->CurrentTile = Tile;

            if (SelectedUnit->OwningBoardManager)
            {
                SelectedUnit->OwningBoardManager->PlayerUnits.Add(SelectedUnit);
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


void ACustomPlayerController::CloseAllUnitInfoWidgets()
{
    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Unit = *It;
        Unit->HideUnitInfo();
    }
}

