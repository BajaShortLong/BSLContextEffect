// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLEffect/BSLEffects/BSLEffect_NiagaraSystem.h"

#include "BSLContextEffectsLibrary.h"
#include "BSLContextEffectsLogging.h"
#include "BSLGameplayTags.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "BSLEffect/BSLEffectParameters.h"
#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_Trace.h"
#include "Kismet/GameplayStatics.h"


UBSLLoadedEffect* UBSLEffectDefinition_NiagaraSystem::MakeLoadedEffect_Implementation()
{
	UBSLEffect_NiagaraSystem* loadedEffect = NewObject<UBSLEffect_NiagaraSystem>(GetOuter());
	loadedEffect->Effect = EffectAsset.ResolveObject();
	loadedEffect->EffectDefinition = this;

	return loadedEffect;
}

TArray<UClass*> UBSLEffectDefinition_NiagaraSystem::GetAllowedAssetClass_Implementation() const
{
	TArray<UClass*> outClass;
	outClass.Add(UNiagaraSystem::StaticClass());
	return outClass;
}

bool UBSLEffect_NiagaraSystem::SpawnEffect(const FBSLContextEffectData& ContextEffectData)
{
	check(ContextEffectData.StaticMeshComponent);
	check(Cast<UNiagaraSystem>(Effect));
	
	UNiagaraComponent* NiagaraComponent;
	AActor* owningActor = ContextEffectData.StaticMeshComponent->GetOwner();

	// Set NiagaraSystem settings from ContextEffectData.CustomData
	FBSLNiagaraSettings niagaraSettings;
	FBSLHitResult physMatHitResult;
	GetNiagaraSettings(ContextEffectData, niagaraSettings, physMatHitResult);

	if (ContextEffectData.bAttached)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CastChecked<UNiagaraSystem>(Effect),
			ContextEffectData.StaticMeshComponent,
			ContextEffectData.Socket,
			ContextEffectData.LocationOffset,
			ContextEffectData.RotationOffset,
			ContextEffectData.ComponentScale,
			niagaraSettings.AttachLocation,
			niagaraSettings.bAutoDestroy,
			niagaraSettings.PoolingMethod,
			niagaraSettings.bAutoActivate,
			niagaraSettings.bPreCullCheck);
	}
	else
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			owningActor,
			CastChecked<UNiagaraSystem>(Effect),
			ContextEffectData.LocationOffset,
			ContextEffectData.RotationOffset,
			ContextEffectData.ComponentScale,
			niagaraSettings.bAutoDestroy,
			niagaraSettings.bAutoActivate,
			niagaraSettings.PoolingMethod,
			niagaraSettings.bPreCullCheck);
	}

	// Pass in physical material if relevant
	if (ContextEffectData.Contexts.HasAny(FGameplayTagContainer(BSLGameplayTags::ContextEffect_Context_SurfaceType)))
	{
		check(physMatHitResult.HitResult.bBlockingHit)
		// Pass physical material type as int to niagara system
		int surface = (int)UGameplayStatics::GetSurfaceType(physMatHitResult.HitResult);
		NiagaraComponent->SetVariableInt(FName("SurfaceType"), surface);
	}

	// Tag spawned effect for future access
	if (!ContextEffectData.LoadedEffectTag.IsNone())
	{
		LoadedEffectTags.AddUnique(ContextEffectData.LoadedEffectTag);
	}

	// Apply all the system parameters to component
	ApplyNiagaraComponentParameters(NiagaraComponent, ContextEffectData);

	EffectComponent = NiagaraComponent;
	
	return true;
}

bool UBSLEffect_NiagaraSystem::DestroyEffect()
{
	if (EffectComponent)
	{
		LoadedEffectTags.Empty();
		EffectComponent->Deactivate();
		return true;
	}
	return false;
}

void UBSLEffect_NiagaraSystem::GetNiagaraSettings(const FBSLContextEffectData& ContextEffectData,
	FBSLNiagaraSettings& OutNiagaraSettings, FBSLHitResult& OutHitResult)
{
	for (const auto& data : ContextEffectData.CustomData)
	{
		if (const FBSLNiagaraSettings* niagaraSettings = data.GetPtr<FBSLNiagaraSettings>())
		{
			OutNiagaraSettings = *niagaraSettings;
#if WITH_EDITORONLY_DATA
			WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set NiagaraSettings for Effect %s on %s"), *GetClientServerContextString(),
				*ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
		}

		if (const FBSLHitResult* hitResult = data.GetPtr<FBSLHitResult>())
		{
			OutHitResult = *hitResult;
#if WITH_EDITORONLY_DATA
			WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set HitResult for Effect %s on %s"), *GetClientServerContextString(),
				*ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
		}
	}
}

void UBSLEffect_NiagaraSystem::ApplyNiagaraComponentParameters(UNiagaraComponent* NiagaraComponent, const FBSLContextEffectData& ContextEffectData) const
{
	for (const auto& param : EffectDefinition->EffectParameters)
	{
		
		if (FBSLEffectParameterFloats::StaticStruct() == param.GetScriptStruct())
		{
			const FBSLEffectParameterFloats& floats = param.Get<FBSLEffectParameterFloats>();
			for (const auto& floatParam : floats.FloatParams)
			{
				NiagaraComponent->SetVariableFloat(floatParam.Key, floatParam.Value);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set float parameter name: %s value: %f for Effect %s on %s"), *GetClientServerContextString(),
					*floatParam.Key.ToString(), floatParam.Value, *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}
		}
		
		if (FBSLEffectParameterBools::StaticStruct() == param.GetScriptStruct())
		{
			FBSLEffectParameterBools bools = param.Get<FBSLEffectParameterBools>();
			for (const auto& boolParam : bools.BoolsParams)
			{
				NiagaraComponent->SetVariableBool(boolParam.Key, boolParam.Value);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set bool parameter name: %s value: %hhd for Effect %s on %s"), *GetClientServerContextString(),
					*boolParam.Key.ToString(), boolParam.Value, *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}			
		}
		
		if (FBSLEffectParameterColors::StaticStruct() == param.GetScriptStruct())
		{
			FBSLEffectParameterColors colors = param.Get<FBSLEffectParameterColors>();
			for (const auto& colorParam : colors.ColorParams)
			{
				UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayColor(NiagaraComponent, colorParam.Key, colorParam.Value.Colors);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set colors parameter name: %s for Effect %s on %s"), *GetClientServerContextString(),
					*colorParam.Key.ToString(), *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}
		}
		
	}
}


