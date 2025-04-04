// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffect/BSLEffectDefinition.h"
#include "BSLEffect_ForceFeedback.generated.h"

class UForceFeedbackComponent;

UCLASS(BlueprintType, DisplayName="Force Feedback Effect")
class BSLCONTEXTEFFECTS_API UBSLEffectDefinition_ForceFeedback : public UBSLEffectDefinition
{
	GENERATED_BODY()

public:

	UBSLEffectDefinition_ForceFeedback();

	virtual UBSLLoadedEffect* MakeLoadedEffect_Implementation() override;
	virtual TArray<UClass*> GetAllowedAssetClass_Implementation() const override;
};

/**
 * 
 */
UCLASS()
class BSLCONTEXTEFFECTS_API UBSLEffect_ForceFeedback : public UBSLLoadedEffect
{
	GENERATED_BODY()

public:

	virtual bool SpawnEffect(const FBSLContextEffectData& ContextEffectData) override;
	virtual bool DestroyEffect() override;

protected:

	void GetForceFeedbackSettings(const FBSLContextEffectData& ContextEffectData, FBSLForceFeedbackSettings& OutForceFeedbackSettings);
	
	// Sets parameters in effect definition
	void ApplyForceFeedbackComponentParameters(UForceFeedbackComponent* ForceFeedbackComponent, const FBSLContextEffectData& ContextEffectData) const;
	
};
