// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffectModifierBase.h"
#include "BSLEffectModifier_InitialVelocity.generated.h"

USTRUCT(BlueprintType)
struct FBSLVelocity
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Velocity = FVector::ZeroVector;
};

/**
 * Injects a FBSLVelocity, populated with owner's current velocity
 */
UCLASS(DisplayName="Initial Velocity")
class BSLCONTEXTEFFECTS_API UBSLEffectModifier_InitialVelocity : public UBSLEffectModifierBase
{
	GENERATED_BODY()

public:

	virtual bool ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const override;
};
