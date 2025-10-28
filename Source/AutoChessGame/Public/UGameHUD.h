#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UGameHUD.generated.h"

UCLASS()
class AUTOCHESSGAME_API UGameHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetRound(int32 Round);

    UFUNCTION(BlueprintCallable)
    void SetPhase(FString PhaseName);

    UFUNCTION(BlueprintCallable)
    void SetResult(FString Result);

};
