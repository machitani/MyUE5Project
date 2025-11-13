// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerManager.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	//プレイヤーを取得してキャスト
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	PlayerManagerRef = Cast<APlayerManager>(Pawn);
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
