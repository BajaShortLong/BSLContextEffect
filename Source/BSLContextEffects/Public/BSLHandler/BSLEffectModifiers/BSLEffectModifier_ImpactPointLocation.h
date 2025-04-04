// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffectModifierBase.h"
#include "BSLEffectModifier_Trace.h"
#include "BSLEffectModifier_ImpactPointLocation.generated.h"

/**
 * Performs a trace using BSLContextTraceSettings and sets the Effect Location to the impact point of the HitResult
 */
UCLASS(DisplayName="ImpactPoint Location")
class BSLCONTEXTEFFECTS_API UBSLEffectModifier_ImpactPointLocation : public UBSLEffectModifier_Trace
{
	GENERATED_BODY()

public:

	virtual bool ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const override;

};
