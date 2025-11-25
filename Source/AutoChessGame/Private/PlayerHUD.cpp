// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "ItemBenchSlot.h"
#include "Unit.h"
#include "Blueprint/DragDropOperation.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerManager.h"

void UPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    // ---- PlayerManager の取得（Actor 版） ----
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerManager::StaticClass(), Found);

    if (Found.Num() > 0)
    {
        PlayerManagerRef = Cast<APlayerManager>(Found[0]);
        UE_LOG(LogTemp, Warning, TEXT("PlayerManagerRef Found: %s"), *GetNameSafe(PlayerManagerRef));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerManagerRef NOT FOUND!!!"));
    }
}


void UPlayerHUD::NativeTick(const FGeometry& MyGeometyr, float DeltaTime)
{

	Super::NativeTick(MyGeometyr, DeltaTime);

	if(!PlayerManagerRef)return;

	//Level
	FString Text = FString::Printf(TEXT("Lv %d"), PlayerManagerRef->PlayerLevel);
	LevelText->SetText(FText::FromString(Text));

	//EXP
	float ExpPercent =
		(float)PlayerManagerRef->CurrentExp /
		(float)PlayerManagerRef->ExpToNextLevel;

	ExpBar->SetPercent(ExpPercent);
}

bool UPlayerHUD::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("[HUD] NativeOnDrop called"));

    if (!InOperation)
    {
        return false;
    }

    // ベンチからのドラッグか確認
    UItemBenchSlot* BenchSlot = Cast<UItemBenchSlot>(InOperation->Payload);
    if (!BenchSlot)
    {
        return false; // 別のドロップは無視
    }

    // ワールド＆PC取得
    UWorld* World = GetWorld();
    if (!World) return false;

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return false;

    // ドロップ時点のマウス座標
    const FVector2D ScreenPos = InDragDropEvent.GetScreenSpacePosition();

    FVector WorldOrigin;
    FVector WorldDir;
    if (!PC->DeprojectScreenPositionToWorld(ScreenPos.X, ScreenPos.Y, WorldOrigin, WorldDir))
    {
        return false;
    }

    // ライントレースでユニットを探す
    FHitResult Hit;
    FVector TraceEnd = WorldOrigin + WorldDir * 10000.0f;

    FCollisionQueryParams Params(FName(TEXT("ItemDropTrace")), false);
    if (World->LineTraceSingleByChannel(Hit, WorldOrigin, TraceEnd, ECC_Visibility, Params))
    {
        if (AUnit* HitUnit = Cast<AUnit>(Hit.GetActor()))
        {
            UE_LOG(LogTemp, Warning, TEXT("[HUD] Drop hit unit: %s"), *HitUnit->GetName());

            // ここでユニットに装備させる
            HitUnit->EquipItem(E_EquiqSlotType::Weapon, BenchSlot->ItemData);

            // ベンチ側を空にしたいならここで処理
            // BenchSlot->ClearSlot();

            return true; // ドロップ処理成功
        }
    }

    return false;

    return false;
}
