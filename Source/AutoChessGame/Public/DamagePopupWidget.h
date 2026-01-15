#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamagePopupWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class AUTOCHESSGAME_API UDamagePopupWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // ダメージ値と種別を渡してセット
    UFUNCTION(BlueprintCallable, Category = "DamagePopup")
    void SetupDamage(float DamageAmount, bool bIsMagicDamage, bool bIsCritical);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamagePopup")
    FLinearColor CritColor = FLinearColor(1.f, 0.9f, 0.3f); // 金色っぽい


    UFUNCTION(BlueprintCallable, Category = "DamagePopup")
    void SetupHeal(float HealAmount);


    UFUNCTION(BlueprintCallable)
    void SetupBuff(const FString& BuffText);

    // 追加の色
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup")
    FLinearColor BuffColor = FLinearColor(1.f, 0.78f, 0.2f, 1.f); // 黄橙


protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DamageText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamagePopup")
    FLinearColor PhysicalColor = FLinearColor(1.f, 0.3f, 0.3f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamagePopup")
    FLinearColor MagicColor = FLinearColor(0.3f, 0.6f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamagePopup")
    FLinearColor HealColor = FLinearColor(0.3f, 1.f, 0.3f);

    // ポップアップの寿命（秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamagePopup")
    float LifeTime = 0.8f;

    // （あれば）ポップアップ演出用アニメ
    UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
    UWidgetAnimation* PopupAnim;

private:
    FTimerHandle LifeTimerHandle;

    UFUNCTION()
    void HandleLifeTimeFinished();
};
