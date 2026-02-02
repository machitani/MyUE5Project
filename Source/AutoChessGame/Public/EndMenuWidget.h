#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndMenuWidget.generated.h"

class UImage; 

UCLASS()
class AUTOCHESSGAME_API UEndMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // true = ClearImage ‚ð•\Ž¦, false = GameOverImage ‚ð•\Ž¦
    UFUNCTION(BlueprintCallable)
    void SetEndTitle(bool bGameClear);

protected:
    // WBP_EndMenu ‚Ì Image –¼‚ð "ClearImage" ‚É‚µ‚Ä‚¨‚­
    UPROPERTY(meta = (BindWidget))
    UImage* ClearImage;

    // WBP_EndMenu ‚Ì Image –¼‚ð "GameOverImage" ‚É‚µ‚Ä‚¨‚­
    UPROPERTY(meta = (BindWidget))
    UImage* GameOverImage;
};
