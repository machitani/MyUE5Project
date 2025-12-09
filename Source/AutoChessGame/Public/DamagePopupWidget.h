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
    void SetupDamage(float DamageAmount, bool bIsMagicDamage);

protected:
    // UMGのTextBlockとバインド
    UPROPERTY(meta = (BindWidget))
    UTextBlock* DamageText;

    // カラー設定（エディタから変えられるようにしておく）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamagePopup")
    FLinearColor PhysicalColor = FLinearColor(1.f, 0.3f, 0.3f);  // 赤っぽい

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamagePopup")
    FLinearColor MagicColor = FLinearColor(0.3f, 0.6f, 1.f);     // 青っぽい

    // ポップアップ用アニメーション
    UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
    UWidgetAnimation* PopupAnim;
};
