#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class AUnit;
class ABoardManager;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class USceneComponent;

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

    /** ハイライト用 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
    UStaticMeshComponent* HighlightMesh;

    /** ユニットがいるか */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
    bool bIsOccupied = false;

    /** このタイルにいるユニット */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
    AUnit* OccupiedUnit = nullptr;

    /** 元の色を保存 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
    FLinearColor OriginalColor;

    /** BoardManager への参照（C++ -> BP で参照可能） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    ABoardManager* BoardManagerRef = nullptr;

    /** 自分側タイルか（敵側なら false） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    bool bIsPlayerTile = true;

    /** ハイライト中か */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
    bool bIsHighlighted = false;

    /** 追加：グリッド座標（7×4のどこか） */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile|Grid")
    int32 Row = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile|Grid")
    int32 Col = -1;

    /** 追加：生成時にまとめて設定（BoardManagerから呼ぶ用） */
    UFUNCTION(BlueprintCallable, Category = "Tile|Grid")
    void InitTile(int32 InRow, int32 InCol, bool bInIsPlayerTile, ABoardManager* InBoardManager);

    /** 追加：占有状態を安全に更新（bIsOccupied と OccupiedUnit を一致させる） */
    UFUNCTION(BlueprintCallable, Category = "Tile|Occupy")
    void SetOccupiedUnit(AUnit* Unit);

    UFUNCTION(BlueprintCallable, Category = "Tile|Occupy")
    void ClearOccupiedUnit();

    UFUNCTION(BlueprintPure, Category = "Tile|Occupy")
    bool IsOccupied() const { return bIsOccupied && OccupiedUnit != nullptr; }

    /** タイルの色を設定 */
    UFUNCTION(BlueprintCallable, Category = "Tile")
    void SetTileColor(const FLinearColor& NewColor);

    /** 元の色に戻す */
    UFUNCTION(BlueprintCallable, Category = "Tile")
    void ResetTileColor();

    /** クリック時に BoardManager に通知 */
    UFUNCTION()
    void NotifyBoardManagerClicked();

    UFUNCTION(BlueprintCallable, Category = "Tile")
    void SetTileHighlight(bool bHighlight);

    FVector GetTileCenterWorld() const;
    float   GetTileSnapRadius() const;

private:
    UPROPERTY()
    UMaterialInstanceDynamic* DynMat = nullptr;

    UPROPERTY()
    USceneComponent* RootScene = nullptr;

    /** 任意：TileMeshクリックでNotify呼ぶ（BP側で拾ってるなら無くてもOK） */
    UFUNCTION()
    void OnTileMeshClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
};
