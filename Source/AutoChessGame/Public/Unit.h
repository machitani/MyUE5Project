#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "BoardManager.h"
#include "Unit.generated.h"

UCLASS()
class AUTOCHESSGAME_API AUnit : public AActor
{
    GENERATED_BODY()

public:
    AUnit();

    /** Mesh */
    UPROPERTY(VisibleAnywhere, Category = "Unit")
    UStaticMeshComponent* UnitMesh;

    /** ドラッグ中フラグ */
    bool bIsDragging;

    /** ドラッグ開始時のマウスワールドとユニットのオフセット */
    FVector DragOffset;

    /** ドラッグ開始 */
    void StartDrag(const FVector& MouseWorld);

    /** ドラッグ終了 */
    void EndDrag();

    /** ドラッグ更新（マウスワールド位置を渡して呼ぶ） */
    void UpdateDrag(const FVector& MouseWorld);

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Unit")
    class ATile* CurrentTile;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
    FVector OriginalLocation;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Unit")
    ABoardManager* OwningBoardManager;
};
