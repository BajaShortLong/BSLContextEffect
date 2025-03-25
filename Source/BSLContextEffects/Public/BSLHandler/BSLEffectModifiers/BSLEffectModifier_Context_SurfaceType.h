// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffectModifier_Trace.h"
#include "BSLEffectModifier_Context_SurfaceType.generated.h"

/**
 * Adds the physical material context tag to ContextEffectData
 */
UCLASS(DisplayName = "Context SurfaceType")
class BSLCONTEXTEFFECTS_API UBSLEffectModifier_Context_SurfaceType : public UBSLEffectModifier_Trace
{
	GENERATED_BODY()

public:

	virtual bool ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const override;
	
	FGameplayTag GetPhysicalSurfaceContextTag(const FBSLContextEffectData& ContextEffectData) const;

};

