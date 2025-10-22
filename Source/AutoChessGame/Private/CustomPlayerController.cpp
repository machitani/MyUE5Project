#include "CustomPlayerController.h"
#include "Engine/World.h"
#include "Unit.h"
#include "Tile.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"

ACustomPlayerController::ACustomPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    bIsDragging = false;
    SelectedUnit = nullptr;

    PrimaryActorTick.bCanEverTick = true;
}

void ACustomPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction("LeftClick", IE_Pressed, this, &ACustomPlayerController::OnLeftMouseDown);
        InputComponent->BindAction("LeftClick", IE_Released, this, &ACustomPlayerController::OnLeftMouseUp);
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

        // === タイルハイライト ===
        FHitResult Hit;
        if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
        {
            ATile* HoveredTile = Cast<ATile>(Hit.GetActor());

            // 前回のハイライト解除
            if (LastHighlightedTile && LastHighlightedTile != HoveredTile)
            {
                LastHighlightedTile->SetTileHighlight(false);
            }

            // 新しいタイルをハイライト
            if (HoveredTile)
            {
                HoveredTile->SetTileHighlight(true);
                LastHighlightedTile = HoveredTile;
            }
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


void ACustomPlayerController::OnLeftMouseDown()
{
    FHitResult Hit;
    // ビジュアルデバッグ用にチャンネルはVisibility
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (Hit.bBlockingHit)
    {
        if (AUnit* Unit = Cast<AUnit>(Hit.GetActor()))
        {
            // 開始位置のワールドポイント（ヒットポイント）
            FVector MouseWorld = Hit.ImpactPoint;
            SelectedUnit = Unit;
            bIsDragging = true;
            SelectedUnit->StartDrag(MouseWorld);

            UE_LOG(LogTemp, Log, TEXT("[Controller] Start dragging %s"), *SelectedUnit->GetName());
            return;
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
        if (ATile* Tile = Cast<ATile>(Hit.GetActor()))
        {
            // もしタイルが埋まってたら配置できないようにする
            if (!Tile->bIsOccupied)
            {
                FVector TargetLoc = Tile->GetActorLocation() + FVector(0, 0, 150.f);
                SelectedUnit->SetActorLocation(TargetLoc);

                // --- タイルとユニットの関連更新 ---
                if (SelectedUnit->CurrentTile)
                {
                    SelectedUnit->CurrentTile->bIsOccupied = false;
                    SelectedUnit->CurrentTile->OccupiedUnit = nullptr;
                }

                SelectedUnit->CurrentTile = Tile;
                Tile->bIsOccupied = true;
                Tile->OccupiedUnit = SelectedUnit;

                UE_LOG(LogTemp, Log, TEXT("[Controller] Dropped %s to Tile %s"), *SelectedUnit->GetName(), *Tile->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[Controller] Tile %s is occupied!"), *Tile->GetName());
            }
        }
        else
        {
            // タイル外に落とした場合
            FVector Impact = Hit.ImpactPoint;
            SelectedUnit->SetActorLocation(FVector(Impact.X, Impact.Y, SelectedUnit->GetActorLocation().Z));
            UE_LOG(LogTemp, Log, TEXT("[Controller] Dropped %s to world location"), *SelectedUnit->GetName());
        }
    }

    SelectedUnit->EndDrag();
    SelectedUnit = nullptr;
}
