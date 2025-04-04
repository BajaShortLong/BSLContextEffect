// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffectModifierBase.h"
#include "BSLEffectModifier_RelativeToMeshComponent.generated.h"

/**
 * Effect modifier that sets the Location and Rotation Offset relative to the valid StaticMeshComponent. 
 */
UCLASS(DisplayName = "Transform Relative To Mesh")
class BSLCONTEXTEFFECTS_API UBSLEffectModifier_RelativeToMeshComponent : public UBSLEffectModifierBase
{
	GENERATED_BODY()

public:

	virtual bool ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const override;
};
