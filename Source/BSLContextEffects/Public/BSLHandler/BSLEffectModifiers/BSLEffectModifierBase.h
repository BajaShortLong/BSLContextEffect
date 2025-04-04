// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffect/BSLEffectData.h"
#include "UObject/Object.h"
#include "BSLEffectModifierBase.generated.h"

class UBSLContextEffectsComponent;
/**
 * Effect Modifiers should never keep any state. They should only be used to modify FBSLContextEffectData passed to them.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType, Abstract)
class BSLCONTEXTEFFECTS_API UBSLEffectModifierBase : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "ContextEffects|EffectModifier")
	bool ModifyEffect(FBSLContextEffectData& ContextEffectData) const;
	
};
