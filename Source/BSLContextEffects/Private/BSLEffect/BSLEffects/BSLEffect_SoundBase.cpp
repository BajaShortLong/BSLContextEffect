// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLEffect/BSLEffects/BSLEffect_SoundBase.h"

#include "BSLContextEffectsLogging.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UBSLLoadedEffect* UBSLEffectDefinition_SoundBase::MakeLoadedEffect_Implementation()
{
	UBSLEffect_SoundBase* loadedEffect = NewObject<UBSLEffect_SoundBase>();
	loadedEffect->Effect = EffectAsset.ResolveObject();
	loadedEffect->EffectDefinition = this;

	return loadedEffect;
}

TArray<UClass*> UBSLEffectDefinition_SoundBase::GetAllowedAssetClass_Implementation() const
{
	TArray<UClass*> outClass;
	outClass.Add(USoundBase::StaticClass());
	return outClass;
}

bool UBSLEffect_SoundBase::SpawnEffect(const FBSLContextEffectData& ContextEffectData)
{
	check(ContextEffectData.StaticMeshComponent);
	check(Cast<USoundBase>(Effect));
	
	UAudioComponent* AudioComponent;
	AActor* owningActor = ContextEffectData.StaticMeshComponent->GetOwner();
	check(owningActor);

	// Set SoundBase settings from ContextEffectData.CustomData
	FBSLSoundSettings soundSettings;
	GetSoundSettings(ContextEffectData, soundSettings);

	if (ContextEffectData.bAttached)
	{
		// Spawn Sounds Attached, add Audio Component to List of ACs
		AudioComponent = UGameplayStatics::SpawnSoundAttached(
			CastChecked<USoundBase>(Effect),
			ContextEffectData.StaticMeshComponent,
			ContextEffectData.Socket,
			ContextEffectData.LocationOffset,
			ContextEffectData.RotationOffset,
			soundSettings.AttachLocation,
			soundSettings.bStopWhenAttachedToDestroyed,
			soundSettings.VolumeMultiplier,
			soundSettings.PitchMultiplier,
			soundSettings.StartTime,
			soundSettings.AttenuationSettings,
			soundSettings.ConcurrencySettings,
			soundSettings.bAutoDestroy);
	} else
	{
		AudioComponent = UGameplayStatics::SpawnSoundAtLocation(
			owningActor,
			CastChecked<USoundBase>(Effect),
			ContextEffectData.LocationOffset,
			ContextEffectData.RotationOffset,
			soundSettings.VolumeMultiplier,
			soundSettings.PitchMultiplier,
			soundSettings.StartTime,
			soundSettings.AttenuationSettings,
			soundSettings.ConcurrencySettings,
			soundSettings.bAutoDestroy);
	}

	// Tag spawned component for future access such as cleanup via AnimNotifyState
	if (!ContextEffectData.LoadedEffectTag.IsNone())
	{
		LoadedEffectTags.AddUnique(ContextEffectData.LoadedEffectTag);
	}

	// Apply all the system parameters to component
	ApplyAudioComponentParameters(AudioComponent, ContextEffectData);

	EffectComponent = AudioComponent;

	return true;
}

bool UBSLEffect_SoundBase::DestroyEffect()
{
	return Super::DestroyEffect();
}

void UBSLEffect_SoundBase::GetSoundSettings(const FBSLContextEffectData& ContextEffectData, FBSLSoundSettings& OutSoundSettings) const
{
	for (const auto& data : ContextEffectData.CustomData)
	{
		if (const FBSLSoundSettings* soundSettings = data.GetPtr<FBSLSoundSettings>())
		{
			OutSoundSettings = *soundSettings;
#if WITH_EDITORONLY_DATA
			WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: got SoundSettings for Effect %s on %s"), *GetClientServerContextString(),
				*ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			return;
		}
	}
}

void UBSLEffect_SoundBase::ApplyAudioComponentParameters(UAudioComponent* AudioComponent,
	const FBSLContextEffectData& ContextEffectData) const
{
}
