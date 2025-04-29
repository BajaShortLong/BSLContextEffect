// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffect/BSLEffectDefinition.h"
#include "Shakes/DefaultCameraShakeBase.h"
#include "BSLEffect_CameraShake.generated.h"

// CameraShakeBase is marked as Abstract(not sure why) and isn't setup to be created as an asset by default.
// BSLCameraShakeBase is a simplistic wrapper class which we use to allow creation of CameraShakeBase assets
UCLASS()
class BSLCONTEXTEFFECTS_API UBSLCameraShakeBase : public UDefaultCameraShakeBase
{
	GENERATED_BODY()
};


UCLASS(BlueprintType, DisplayName="Camera Shake Effect")
class BSLCONTEXTEFFECTS_API UBSLEffectDefinition_CameraShake : public UBSLEffectDefinition
{
	GENERATED_BODY()

public:

	UBSLEffectDefinition_CameraShake();

	virtual UBSLLoadedEffect* MakeLoadedEffect_Implementation() override;
	virtual TArray<UClass*> GetAllowedAssetClass_Implementation() const override;

	// Camera shakes pass in a class instead of object so we need to set this instead of EffectObject
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSoftClassPath EffectClass;
};

/**
 * 
 */
UCLASS()
class BSLCONTEXTEFFECTS_API UBSLEffect_CameraShake : public UBSLLoadedEffect
{
	GENERATED_BODY()

public:

	virtual bool SpawnEffect(const FBSLContextEffectData& ContextEffectData) override;
	virtual bool DestroyEffect() override;

protected:

	void GetCameraShakeSettings(const FBSLContextEffectData& ContextEffectData, FBSLCameraShakeSettings& OutCameraShakeSettings);

protected:

	FTimerHandle DestructionTimer;
	
};

