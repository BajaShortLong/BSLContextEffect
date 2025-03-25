// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffectModifierBase.h"
#include "BSLEffectModifier_Trace.generated.h"


USTRUCT(BlueprintType)
struct BSLCONTEXTEFFECTS_API FBSLContextTraceSettings
{
	GENERATED_BODY()

	static const FBSLContextTraceSettings EmptySettings;

	// Trace Channel
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Visibility;

	// Vector offset for start location
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartTraceLocationOffset = FVector(0,0,10);
	
	// Vector offset from Effect Location
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector EndTraceLocationOffset = FVector(0,0,-50);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReturnPhysicalMaterial = false;

	// Ignore this Actor when getting trace result
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIgnoreActor = true;
};

USTRUCT(BlueprintType)
struct FBSLHitResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult HitResult;
};

/**
 * 
 */
UCLASS(DisplayName = "Line Trace")
class BSLCONTEXTEFFECTS_API UBSLEffectModifier_Trace : public UBSLEffectModifierBase
{
	GENERATED_BODY()

public:

	virtual bool ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const;
	
	const FBSLContextTraceSettings& GetTraceSettings(const FBSLContextEffectData& ContextEffectData) const;

	// Adds a FHitResult to ContextEffectData.CustomData
	void SetContextEffectDataHitResult(const FHitResult& InHitResult, FBSLContextEffectData& ContextEffectData) const;

	void PerformTrace(const FBSLContextEffectData& ContextEffectData, const FBSLContextTraceSettings& InTraceSettings, FHitResult& OutHitResult) const;
	
};
