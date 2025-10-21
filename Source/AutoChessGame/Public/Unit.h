#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"

UCLASS()
class AUTOCHESSGAME_API AUnit : public AActor
{
    GENERATED_BODY()

public:
    AUnit();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

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
};
