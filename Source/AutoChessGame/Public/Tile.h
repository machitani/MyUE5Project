#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class AUnit;

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

    /** �^�C���̃��b�V�� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
    UStaticMeshComponent* TileMesh;

    /** ���̃}�X�Ƀ��j�b�g�����邩 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    bool bIsOccupied;

    /** ���̃^�C���ɂ��郆�j�b�g */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
    AUnit* OccupiedUnit;

    /** ���̐F */
    FLinearColor OriginalColor;

    /** �^�C���̐F��ݒ� */
    UFUNCTION(BlueprintCallable, Category = "Tile")
    void SetTileColor(const FLinearColor& NewColor);

    /** ���̐F�ɖ߂� */
    UFUNCTION(BlueprintCallable, Category = "Tile")
    void ResetTileColor();
};
