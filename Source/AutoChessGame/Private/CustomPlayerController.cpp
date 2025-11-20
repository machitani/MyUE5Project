#include "CustomPlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
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

    if (!Hit.bBlockingHit)return;

    //ユニットを右クリック
    if (Hit.bBlockingHit)
    {
        if (AUnit* Unit = Cast<AUnit>(Hit.GetActor()))
        {
            Unit->ShowUnitInfo();  // ← 表示
            return;
        }
    }
    CloseAllUnitInfoWidgets();
}

void ACustomPlayerController::CloseAllUnitInfoWidgets()
{
    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Unit = *It;
        Unit->HideUnitInfo();
    }
}

