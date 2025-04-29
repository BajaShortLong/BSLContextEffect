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
	FBSLVelocity initialVelocity;
	GetNiagaraSettings(ContextEffectData, niagaraSettings, physMatHitResult, initialVelocity);

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

	// Set Velocity if applicable
	if (!initialVelocity.Velocity.IsZero())
	{
		NiagaraComponent->SetVectorParameter(FName("InitialVelocity"), initialVelocity.Velocity);
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
	FBSLNiagaraSettings& OutNiagaraSettings, FBSLHitResult& OutHitResult, FBSLVelocity& OutVelocity)
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

		if (const FBSLVelocity* initialVelocity = data.GetPtr<FBSLVelocity>())
		{
			OutVelocity = *initialVelocity;
#if WITH_EDITORONLY_DATA
			WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set InitialVelocity for Effect %s on %s"), *GetClientServerContextString(),
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
			for (const auto& boolParam : bools.BoolParams)
			{
				NiagaraComponent->SetVariableBool(boolParam.Key, boolParam.Value);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set bool parameter name: %s value: %hhd for Effect %s on %s"), *GetClientServerContextString(),
					*boolParam.Key.ToString(), boolParam.Value, *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}			
		}

		if (FBSLEffectParameterInts::StaticStruct() == param.GetScriptStruct())
		{
			FBSLEffectParameterInts ints = param.Get<FBSLEffectParameterInts>();
			for (const auto& intParam : ints.IntParams)
			{
				NiagaraComponent->SetVariableInt(intParam.Key, intParam.Value);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set int parameter name: %s value: %hhd for Effect %s on %s"), *GetClientServerContextString(),
					*intParam.Key.ToString(), intParam.Value, *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}			
		}

		if (FBSLEffectParameterVectors::StaticStruct() == param.GetScriptStruct())
		{
			FBSLEffectParameterVectors vectors = param.Get<FBSLEffectParameterVectors>();
			for (const auto& vectorParam : vectors.VectorParams)
			{
				NiagaraComponent->SetVariableVec3(vectorParam.Key, vectorParam.Value);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set vector parameter name: %s value: %s for Effect %s on %s"), *GetClientServerContextString(),
					*vectorParam.Key.ToString(), *vectorParam.Value.ToString(), *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}			
		}
		
		if (FBSLEffectParameterTextures::StaticStruct() == param.GetScriptStruct())
		{
			FBSLEffectParameterTextures textures = param.Get<FBSLEffectParameterTextures>();
			for (const auto& textureParam : textures.TextureParams)
			{
				NiagaraComponent->SetVariableTexture(textureParam.Key, textureParam.Value);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set int parameter name: %s value: %s for Effect %s on %s"), *GetClientServerContextString(),
					*textureParam.Key.ToString(), *textureParam.Value.GetName(), *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}			
		}

		if (FBSLEffectParameterColors::StaticStruct() == param.GetScriptStruct())
		{
			FBSLEffectParameterColors colors = param.Get<FBSLEffectParameterColors>();
			for (const auto& colorParam : colors.ColorParams)
			{
				NiagaraComponent->SetVariableLinearColor(colorParam.Key, colorParam.Value);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set int parameter name: %s value: %s for Effect %s on %s"), *GetClientServerContextString(),
					*colorParam.Key.ToString(), *colorParam.Value.ToString(), *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}			
		}

		if (FBSLEffectParameterColorArrays::StaticStruct() == param.GetScriptStruct())
		{
			FBSLEffectParameterColorArrays colors = param.Get<FBSLEffectParameterColorArrays>();
			for (const auto& colorParam : colors.ColorArrayParams)
			{
				UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayColor(NiagaraComponent, colorParam.Key, colorParam.Value.Colors);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set colors parameter name: %s for Effect %s on %s"), *GetClientServerContextString(),
					*colorParam.Key.ToString(), *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}
		}
		
		if (FBSLEffectParameterColorArrays::StaticStruct() == param.GetScriptStruct())
		{
			FBSLEffectParameterColorArrays colors = param.Get<FBSLEffectParameterColorArrays>();
			for (const auto& colorParam : colors.ColorArrayParams)
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


