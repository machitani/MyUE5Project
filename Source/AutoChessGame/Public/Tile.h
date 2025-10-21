#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class AUnit;
class ABoardManager;

UCLASS()
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

    /** このマスにユニットがいるか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    bool bIsOccupied;

    /** このタイルにいるユニット */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    AUnit* OccupiedUnit;

    /** 元の色 */
    FLinearColor OriginalColor;

    /** タイルの色を設定 */
    UFUNCTION(BlueprintCallable, Category = "Tile")
    void SetTileColor(const FLinearColor& NewColor);

    /** 元の色に戻す */
    UFUNCTION(BlueprintCallable, Category = "Tile")
    void ResetTileColor();

    UPROPERTY()
    ABoardManager* BoardManagerRef;

};
