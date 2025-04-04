// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffect/BSLEffectDefinition.h"
#include "BSLEffect_SoundBase.generated.h"

UCLASS(BlueprintType, DisplayName="Sound Effect")
class BSLCONTEXTEFFECTS_API UBSLEffectDefinition_SoundBase : public UBSLEffectDefinition
{
	GENERATED_BODY()

public:

	virtual UBSLLoadedEffect* MakeLoadedEffect_Implementation() override;
	virtual TArray<UClass*> GetAllowedAssetClass_Implementation() const override;
};

/**
 * 
 */
UCLASS()
class BSLCONTEXTEFFECTS_API UBSLEffect_SoundBase : public UBSLLoadedEffect
{
	GENERATED_BODY()

public:

	virtual bool SpawnEffect(const FBSLContextEffectData& ContextEffectData) override;
	virtual bool DestroyEffect() override;

protected:

	void GetSoundSettings(const FBSLContextEffectData& ContextEffectData, FBSLSoundSettings& OutSoundSettings) const;

	// Sets parameters in effect definition
	void ApplyAudioComponentParameters(UAudioComponent* AudioComponent, const FBSLContextEffectData& ContextEffectData) const;
	
};
