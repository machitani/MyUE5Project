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

            // 簡易的にカメラからの距離で位置決め（環境によって 500 を調整）
            FVector MouseWorldPoint = WorldOrigin + WorldDir * 500.f;
            SelectedUnit->UpdateDrag(MouseWorldPoint);
        }
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

    // ドロップ先の確認
    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (Hit.bBlockingHit)
    {
        if (ATile* Tile = Cast<ATile>(Hit.GetActor()))
        {
            FVector TargetLoc = Tile->GetActorLocation();
            SelectedUnit->SetActorLocation(TargetLoc + FVector(0, 0, 100.f)); // 少し浮かせる
            UE_LOG(LogTemp, Log, TEXT("[Controller] Dropped %s to Tile %s"), *SelectedUnit->GetName(), *Tile->GetName());
        }
        else
        {
            // タイルでないところに落としたらそのままその位置に落とす
            FVector Impact = Hit.ImpactPoint;
            SelectedUnit->SetActorLocation(FVector(Impact.X, Impact.Y, SelectedUnit->GetActorLocation().Z));
            UE_LOG(LogTemp, Log, TEXT("[Controller] Dropped %s to world location"), *SelectedUnit->GetName());
        }
    }

    SelectedUnit->EndDrag();
    SelectedUnit = nullptr;
}
