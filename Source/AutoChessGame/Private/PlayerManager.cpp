// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerManager.h"
#include "LevelUpRewardWidget.h"
#include "Unit.h"
#include "BoardManager.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
APlayerManager::APlayerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void APlayerManager::AddItem(const FItemData& NewItem)
{
    OwnedItems.Add(NewItem);
    //UE_LOG(LogTemp, Log, TEXT("Item added: %s"), *NewItem.ItemName.ToString());
}

void APlayerManager::AddExp(int32 Amount)
{
    CurrentExp += Amount;

    const int32 MaxPlayerLevel = 7;
    const int32 MaxUnitsHardCap = 8;

    while (CurrentExp >= ExpToNextLevel && PlayerLevel < MaxPlayerLevel)
    {
        CurrentExp -= ExpToNextLevel;
        PlayerLevel++;

        ExpToNextLevel += 4;

        if (MaxUnitCount < MaxUnitsHardCap)
        {
            MaxUnitCount++;
        }

        OnLevelUp();
    }

    if (PlayerLevel >= MaxPlayerLevel)
    {
        CurrentExp = 0;
    }
}

void APlayerManager::OnLevelUp()
{
    UE_LOG(LogTemp, Warning, TEXT("LEVEL UP! Level: %d"), PlayerLevel);

    // UIクラス未設定なら何もしない
    if (!LevelUpRewardWidgetClass) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    ULevelUpRewardWidget* RewardWidget =
        CreateWidget<ULevelUpRewardWidget>(PC, LevelUpRewardWidgetClass);

    if (!RewardWidget) return;

    RewardWidget->OwnerPlayerManager = this;

    // ★ とりあえず候補3つを決める（仮）
    TArray<FName> Candidates;
    Candidates.Add(FName("Nurse"));
    Candidates.Add(FName("Bear"));
    Candidates.Add(FName("Adventurer"));

    // TODO: 本当はランダム抽選にしてもいい

    RewardWidget->SetupChoices(Candidates);
    RewardWidget->AddToViewport();

}

void APlayerManager::OnRewardSelected(FName SelectedUnitID)
{
    UE_LOG(LogTemp, Warning, TEXT("Reward Selected: %s"), *SelectedUnitID.ToString());

    // （お好み）手持ちリストに保存しておきたい場合
    OwnedUnitIDs.Add(SelectedUnitID);

    // ★ BoardManager に「このユニットを出して」とお願いする
    if (BoardManagerRef)
    {
        AUnit* Spawned = BoardManagerRef->SpawnRewardUnit(SelectedUnitID);
        if (!Spawned)
        {
            UE_LOG(LogTemp, Warning, TEXT("OnRewardSelected: Failed to spawn unit on board."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OnRewardSelected: BoardManagerRef is null!"));
    }
}

