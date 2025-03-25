// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffect/BSLEffectData.h"
#include "BSLEffect/BSLEffectDefinition.h"
#include "UObject/Interface.h"
#include "BSLHandlerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UBSLHandlerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Handlers modify, route, and help coordinate lifecycles of context effects
 */
class BSLCONTEXTEFFECTS_API IBSLHandlerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HandleStartEffect(FBSLContextEffectData& EffectData);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HandleStartEffectRef(const FBSLContextEffectData& EffectData);

	/*UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HandleStartEffectCopy(FBSLContextEffectData EffectData);*/

	// Requires a component tag that matches the system
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HandleEndEffect(FName LoadedEffectTag);
 	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FGameplayTagContainer GetDebugTags();
};
