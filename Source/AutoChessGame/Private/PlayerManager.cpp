// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerManager.h"
#include "LevelUpRewardWidget.h"
#include "Unit.h"
#include "BoardManager.h"
#include "Kismet/GameplayStatics.h"
#include "CustomPlayerController.h"



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

    int32 LevelsGained = 0;

    while (CurrentExp >= ExpToNextLevel && PlayerLevel < MaxPlayerLevel)
    {
        CurrentExp -= ExpToNextLevel;
        PlayerLevel++;

        ExpToNextLevel += 4;

        if (MaxUnitCount < MaxUnitsHardCap)
        {
            MaxUnitCount++;
        }

        LevelsGained++;
    }

    if (LevelsGained > 0)
    {
        PendingLevelUpRewards += LevelsGained;
        UE_LOG(LogTemp, Warning, TEXT("LEVEL UP! Level=%d Gained=%d PendingRewards=%d"),
            PlayerLevel, LevelsGained, PendingLevelUpRewards);

        TryOpenNextLevelUpReward();
    }

    if (PlayerLevel >= MaxPlayerLevel)
    {
        CurrentExp = 0;
    }
}

void APlayerManager::OnLevelUp()
{
    UE_LOG(LogTemp, Warning, TEXT("LEVEL UP! Level: %d"), PlayerLevel);

    if (!BoardManagerRef) return;

    TArray<FName> Candidates = BoardManagerRef->GenerateRewardUnitCandidates(3);
    if (Candidates.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnLevelUp: No reward candidates"));
        return;
    }

    if (ACustomPlayerController* CPC =
        Cast<ACustomPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        CPC->PlayLevelUpUI();
        CPC->ShowLevelUpRewardUI(this, Candidates); // ÅöêVÇµÇ≠çÏÇÈä÷êîÅiâ∫Åj
    }
}

void APlayerManager::OnRewardSelected(FName SelectedUnitID)
{
    UE_LOG(LogTemp, Warning, TEXT("Reward Selected: %s"), *SelectedUnitID.ToString());

    RegisterOwnedUnit(SelectedUnitID);

    if (BoardManagerRef)
    {
        BoardManagerRef->SpawnRewardUnit(SelectedUnitID);
    }
    PendingLevelUpRewards = FMath::Max(0, PendingLevelUpRewards - 1);

    // ÅöÇ±ÇÍÇç≈å„Ç…í«â¡
    if (ACustomPlayerController* CPC =
        Cast<ACustomPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        CPC->EndLevelUpRewardUI();
    }
    TryOpenNextLevelUpReward();
}

void APlayerManager::RegisterOwnedUnit(FName UnitID)
{
    if (UnitID.IsNone())return;

    if (!OwnedUnitIDs.Contains(UnitID))
    {
        OwnedUnitIDs.Add(UnitID);
    }
}

void APlayerManager::TryOpenNextLevelUpReward()
{
    if (!BoardManagerRef) return;
    if (PendingLevelUpRewards <= 0) return;

    ACustomPlayerController* CPC =
        Cast<ACustomPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    if (!CPC) return;

    // Ç∑Ç≈Ç…UIíÜÇ»ÇÁéüâÒÇ…âÒÇ∑
    if (CPC->IsLevelUpLockingInput()) return;

    TArray<FName> Candidates = BoardManagerRef->GenerateRewardUnitCandidates(3);
    if (Candidates.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("TryOpenNextLevelUpReward: No candidates"));
        return;
    }

    CPC->PlayLevelUpUI();
    CPC->ShowLevelUpRewardUI(this, Candidates);
}

bool APlayerManager::IsLevelMax() const
{
    return PlayerLevel >= GetMaxPlayerLevel();
}

int32 APlayerManager::GetMaxPlayerLevel() const
{
    return 7;
}

