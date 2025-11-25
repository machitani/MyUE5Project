#include "CustomPlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Unit.h"
#include "BoardManager.h"
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
        InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ACustomPlayerController::OnLeftClick);
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

void ACustomPlayerController::OnLeftClick()
{
    float MouseX = 0.f, MouseY = 0.f;
    if (!GetMousePosition(MouseX, MouseY))
    {
        UE_LOG(LogTemp, Warning, TEXT("[PC] GetMousePosition failed"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[PC] OnLeftClick at (%.1f, %.1f)"), MouseX, MouseY);

    FVector WorldOrigin, WorldDir;
    if (!DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDir))
    {
        UE_LOG(LogTemp, Warning, TEXT("[PC] Deproject failed"));
        return;
    }

    FVector TraceEnd = WorldOrigin + WorldDir * 10000.f;

    FCollisionQueryParams Params(FName(TEXT("LeftClickTrace")), false);
    if (APawn* MyPawn = GetPawn())
    {
        Params.AddIgnoredActor(MyPawn);
    }

    FHitResult Hit;
    if (!GetWorld()->LineTraceSingleByChannel(
        Hit,
        WorldOrigin,
        TraceEnd,
        ECC_Visibility,
        Params))
    {
        UE_LOG(LogTemp, Warning, TEXT("[PC] Click trace hit nothing"));
        return;
    }

    AActor* HitActor = Hit.GetActor();
    if (!HitActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PC] HitActor is null"));
        return;
    }

    // ① 直接ユニットをクリックした場合
    if (AUnit* HitUnit = Cast<AUnit>(HitActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("[PC] HitUnit = %s"), *HitUnit->GetName());

        if (ABoardManager* BM = Cast<ABoardManager>(
            UGameplayStatics::GetActorOfClass(this, ABoardManager::StaticClass())))
        {
            BM->ItemUnit = HitUnit;
            UE_LOG(LogTemp, Warning,
                TEXT("[PC] Set ItemUnit = %s on %s (direct AUnit)"),
                *HitUnit->GetName(),
                *BM->GetName());
        }
        return;
    }

    // ② タイルをクリックした場合 → タイルに乗っているユニットを使う
    if (ATile* HitTile = Cast<ATile>(HitActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("[PC] Hit Tile = %s"), *HitTile->GetName());

        AUnit* UnitOnTile = Cast<AUnit>(HitTile->OccupiedUnit);
        if (UnitOnTile)
        {
            UE_LOG(LogTemp, Warning, TEXT("[PC] Tile has Unit = %s"), *UnitOnTile->GetName());

            if (ABoardManager* BM = Cast<ABoardManager>(
                UGameplayStatics::GetActorOfClass(this, ABoardManager::StaticClass())))
            {
                BM->ItemUnit = UnitOnTile;
                UE_LOG(LogTemp, Warning,
                    TEXT("[PC] Set ItemUnit = %s on %s (from Tile)"),
                    *UnitOnTile->GetName(),
                    *BM->GetName());
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[PC] Tile has no OccupiedUnit"));
        }
        return;
    }

    // ③ それ以外のオブジェクトに当たった
    UE_LOG(LogTemp, Warning,
        TEXT("[PC] Hit %s (not AUnit/Tile)"),
        *HitActor->GetName());
}


void ACustomPlayerController::CloseAllUnitInfoWidgets()
{
    for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
    {
        AUnit* Unit = *It;
        Unit->HideUnitInfo();
    }
}

