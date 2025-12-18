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
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UProgressBar* HPBar;

    // ユニットへの参照
    UPROPERTY(BlueprintReadWrite)
    class AUnit* OwnerUnit;

    virtual void NativeTick(
        const FGeometry& MyGeometry,
        float InDeltaTime
    ) override;
};
