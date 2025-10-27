#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "BoardManager.h"
#include "Unit.generated.h"

UENUM(BlueprintType)
enum class EUnitTeam : uint8
{
    Player UMETA(DisplayName = "Player"),
    Enemy  UMETA(DisplayName = "Enemy")
};

UCLASS()
class AUTOCHESSGAME_API AUnit : public AActor
{
    GENERATED_BODY()

public:
    AUnit();

    /** Mesh */
    UPROPERTY(VisibleAnywhere, Category = "Unit")
    UStaticMeshComponent* UnitMesh;

    /** �h���b�O���t���O */
    bool bIsDragging;

    /** �h���b�O�J�n���̃}�E�X���[���h�ƃ��j�b�g�̃I�t�Z�b�g */
    FVector DragOffset;

    /** �h���b�O�J�n */
    void StartDrag(const FVector& MouseWorld);

    /** �h���b�O�I�� */
    void EndDrag();

    /** �h���b�O�X�V�i�}�E�X���[���h�ʒu��n���ČĂԁj */
    void UpdateDrag(const FVector& MouseWorld);

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Unit")
    class ATile* CurrentTile;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
    FVector OriginalLocation;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Unit")
    ABoardManager* OwningBoardManager;

    //���j�b�g�̃X�e�[�^�X
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float MaxHP = 100.f;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,CateGory="Stats")
    float HP = 100.f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float Attack = 20.f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Stats")
    float MoveSpeed = 300.f;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Stats")
    float Range = 300.f;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Stats")
    int32 Level = 1;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
    float AttackInterval = 1.0f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Unit")
    bool bCanDrag = true;

    float TimeSinceLastAttack = 0.0f;

    virtual void Tick(float DeltaTime)override;

    void CheckForTarget(const float DeltaTime);
    void AttackTarget(AUnit* Targer);
    void OnDeath();

    

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    EUnitTeam Team = EUnitTeam::Player;  // �f�t�H���g�̓v���C���[
};
