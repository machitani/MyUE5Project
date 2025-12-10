#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class AUnit;
class ABoardManager;

UCLASS(Blueprintable)
class AUTOCHESSGAME_API ATile : public AActor
{
    GENERATED_BODY()

public:
    ATile();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    /** タイルのメッシュ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
    UStaticMeshComponent* TileMesh;

    /** ユニットがいるか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    bool bIsOccupied;

    /** このタイルにいるユニット */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    AUnit* OccupiedUnit;

    /** 元の色を保存 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
    FLinearColor OriginalColor;

    /** BoardManager への参照（C++ -> BP で参照可能） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    ABoardManager* BoardManagerRef;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Tile")
    UStaticMeshComponent* HighlightMesh;

    /** タイルの色を設定 */
    UFUNCTION(BlueprintCallable, Category = "Tile")
    void SetTileColor(const FLinearColor& NewColor);

    /** 元の色に戻す */
    UFUNCTION(BlueprintCallable, Category = "Tile")
    void ResetTileColor();

    /** クリック時に BoardManager に通知 */
    UFUNCTION()
    void NotifyBoardManagerClicked();

    UFUNCTION()
    void SetTileHighlight(bool bHighlight);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    bool bIsPlayerTile;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsHighlighted;

    FVector GetTileCenterWorld() const;
    float   GetTileSnapRadius() const;

private:
    UMaterialInstanceDynamic* DynMat;
};
