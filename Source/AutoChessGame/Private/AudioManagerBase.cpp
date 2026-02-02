#include "AudioManagerBase.h"
#include "Components/AudioComponent.h"

AAudioManagerBase::AAudioManagerBase()
{
    PrimaryActorTick.bCanEverTick = false;

    BGMComp = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComp"));
    SetRootComponent(BGMComp);

    BGMComp->bAutoActivate = false;
    BGMComp->bIsUISound = true; // BGM/UIˆµ‚¢‚É‚µ‚½‚¢‚È‚ç
}

void AAudioManagerBase::SwitchBGM(USoundBase* NewSound)
{
    if (!BGMComp || !NewSound) return;

    // “¯‚¶‹È‚ª‚·‚Å‚É–Â‚Á‚Ä‚½‚ç–³‘Ê‚ÉØ‚è‘Ö‚¦‚È‚¢
    if (BGMComp->Sound == NewSound && BGMComp->IsPlaying())
        return;

    if (BGMComp->IsPlaying())
    {
        BGMComp->FadeOut(FadeTime, 0.0f);
    }

    BGMComp->SetSound(NewSound);
    BGMComp->FadeIn(FadeTime, 1.0f, 0.0f);
}
