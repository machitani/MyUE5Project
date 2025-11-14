// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerManager.h"

void UPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    // ---- PlayerManager ‚Ìæ“¾iActor ”Åj ----
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
