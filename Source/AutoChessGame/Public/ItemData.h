#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

// �� DataTable�Ɏg���\���́B�e�A�C�e���̊�{���B
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY()

public:

    // �A�C�e���̓���ID�i��F"ATK_UP"�j
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FName ItemID;

    // �\�����i�v���C���[�����閼�O�j
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FText ItemName;

    // �������i�A�C�e�����ʂ̃e�L�X�g�j
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FText Description;

    // �V���b�v�ł̍w���R�X�g
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    int32 Cost = 1;

    // ���A���e�B�i1=�m�[�}���A2=���A�A3=SR�A4=UR�Ȃǁj
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    int32 Rarity = 1;

    // �A�C�R���摜�iUMG�ŕ\���j
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    UTexture2D* Icon = nullptr;
};
