#pragma once
#include "EquiqSlotType.generated.h"

UENUM(BlueprintType)
enum class E_EquiqSlotType : uint8
{
    Weapon     UMETA(DisplayName = "Weapon"),
    Armor      UMETA(DisplayName = "Armor"),
    Accessory  UMETA(DisplayName = "Accessory")
};
