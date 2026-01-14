#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Tile.h"
#include "Unit.h"
#include "EquiqSlotType.h"
#include "UnitEquiqSlot.h"
#include "CustomPlayerController.generated.h"

class UUserWidget;
class ABoradManager;
class APlayerManager;
class ULevelUpRewardWidget;

UCLASS()
class AUTOCHESSGAME_API ACustomPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ACustomPlayerController();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> TitleWidgetClass;

    UPROPERTY(EditDefaultsOnly,Category="UI")
    TSubclassOf<UUserWidget>OverlayClass;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void RequestBattleStartUI(ABoardManager* BoardManager);

    UFUNCTION(BlueprintCallable,Category="UI")
    void OnGameStartUIFinished();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void OnBattleStartUIFinished();

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuClass;

    UPROPERTY()
    UUserWidget* PauseMenuInstance = nullptr;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void TogglePauseMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ResumeGame();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ReturnToTitle();

    UPROPERTY()
    UUserWidget* TitleWidgetInstance = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> EndMenuClass;

    UPROPERTY()
    UUserWidget* EndMenuInstance = nullptr;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowEndMenu(bool bGameClear);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideEndMenu();

    UPROPERTY(EditAnywhere,Category="UI")
    TSubclassOf<UUserWidget>LevelUpRewardClass;

    UFUNCTION(BlueprintCallable,Category="UI")
    void ShowLevelUpRewardUI(APlayerManager* PM,const TArray<FName>&Candidates);

    UFUNCTION(BlueprintCallable,Category="UI")
    void OnLevelUpRewardPicked(FName UnitID);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void EndLevelUpRewardUI();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void PlayLevelUpUI();

protected:
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void BeginPlay()override;

private:
    bool bIsDragging;
    AUnit* SelectedUnit;

    void OnLeftMouseDown();
    void OnLeftMouseUp();
    void OnRightClick();
    void OnLeftClick();
    UFUNCTION(BlueprintCallable,Category="UI")
    void EnterTitleMode();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void EnterGameMode();

   

    void CloseAllUnitInfoWidgets();

    ATile* LastHighlightedTile;

    UPROPERTY()
    UUserWidget* OverlayWidget = nullptr;

    TWeakObjectPtr<ABoardManager> PendingBoardManager;

    void LockInputForIntro(bool bLock);
    void CallOverlayEventByName(FName EventName);

    UPROPERTY()
    UUserWidget* LevelUpRewardWidget = nullptr;

    bool bIsLevelUpChoosing = false;
};
