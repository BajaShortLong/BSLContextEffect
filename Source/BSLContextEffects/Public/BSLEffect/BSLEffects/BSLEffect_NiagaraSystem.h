// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "BSLEffect/BSLEffectDefinition.h"
#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_InitialVelocity.h"
#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_Trace.h"
#include "BSLEffect_NiagaraSystem.generated.h"

UCLASS(BlueprintType, DisplayName="Niagara Particle Effect")
class BSLCONTEXTEFFECTS_API UBSLEffectDefinition_NiagaraSystem : public UBSLEffectDefinition
{
	GENERATED_BODY()

public:

	virtual UBSLLoadedEffect* MakeLoadedEffect_Implementation() override;
	virtual TArray<UClass*> GetAllowedAssetClass_Implementation() const override;
};

class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class BSLCONTEXTEFFECTS_API UBSLEffect_NiagaraSystem : public UBSLLoadedEffect
{
	GENERATED_BODY()

public:

	virtual bool SpawnEffect(const FBSLContextEffectData& ContextEffectData) override;
	virtual bool DestroyEffect() override;

protected:

	void GetNiagaraSettings(const FBSLContextEffectData& ContextEffectData, FBSLNiagaraSettings& OutNiagaraSettings,
		FBSLHitResult& OutHitResult, FBSLVelocity& OutVelocity);
	
	// Sets parameters in effect definition
	void ApplyNiagaraComponentParameters(UNiagaraComponent* NiagaraComponent, const FBSLContextEffectData& ContextEffectData) const;
	
};
