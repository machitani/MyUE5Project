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

    TArray<FName> Candidates;

    if (BoardManagerRef)
    {
        // 例: 3体の候補をランダム取得
        Candidates = BoardManagerRef->GenerateRewardUnitCandidates(3);
    }

    // 候補が1つも無い場合（全ユニット取り切った等）は何もしない or メッセージ表示
    if (Candidates.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnLevelUp: No reward candidates (all units owned?)"));
        return;
    }

    RewardWidget->SetupChoices(Candidates);
    RewardWidget->AddToViewport();

}

void APlayerManager::OnRewardSelected(FName SelectedUnitID)
{
    UE_LOG(LogTemp, Warning, TEXT("Reward Selected: %s"), *SelectedUnitID.ToString());

    // ★ 所持ユニットとして登録（重複チェック込み）
    RegisterOwnedUnit(SelectedUnitID);

    // BoardManager に「このユニットを出して」とお願い
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

void APlayerManager::RegisterOwnedUnit(FName UnitID)
{
    if (UnitID.IsNone())return;

    if (!OwnedUnitIDs.Contains(UnitID))
    {
        OwnedUnitIDs.Add(UnitID);
    }
}

