#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "AudioManagerBase.generated.h"

class UAudioComponent;

UCLASS()
class AUTOCHESSGAME_API AAudioManagerBase : public AActor
{
    GENERATED_BODY()

public:
    AAudioManagerBase();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SwitchBGM(USoundBase* NewSound);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
    UAudioComponent* BGMComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float FadeTime = 0.3f;
};
