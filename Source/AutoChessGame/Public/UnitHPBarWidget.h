// UnitHPBarWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitHPBarWidget.generated.h"

class UProgressBar;
class AUnit;

UCLASS()
class AUTOCHESSGAME_API UUnitHPBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // BP側の ProgressBar と自動バインドする
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HPBar;

    // このバーが参照するユニット
    UPROPERTY(BlueprintReadWrite, Category = "Unit")
    AUnit* OwnerUnit;

protected:
    //virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
