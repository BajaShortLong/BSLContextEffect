// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLEffect/BSLEffects/BSLEffect_ForceFeedback.h"

#include "BSLContextEffectsLibrary.h"
#include "BSLContextEffectsLogging.h"
#include "BSLEffect/BSLEffectParameters.h"
#include "Components/ForceFeedbackComponent.h"
#include "Kismet/GameplayStatics.h"


UBSLEffectDefinition_ForceFeedback::UBSLEffectDefinition_ForceFeedback()
{
	bLocallyControlledOnly = true;
}

UBSLLoadedEffect* UBSLEffectDefinition_ForceFeedback::MakeLoadedEffect_Implementation()
{
	UBSLEffect_ForceFeedback* loadedEffect = NewObject<UBSLEffect_ForceFeedback>(GetOuter());
	loadedEffect->Effect = EffectAsset.ResolveObject();
	loadedEffect->EffectDefinition = this;

	return loadedEffect;
}

TArray<UClass*> UBSLEffectDefinition_ForceFeedback::GetAllowedAssetClass_Implementation() const
{
	TArray<UClass*> outClass;
	outClass.Add(UForceFeedbackEffect::StaticClass());
	return outClass;
}

bool UBSLEffect_ForceFeedback::SpawnEffect(const FBSLContextEffectData& ContextEffectData)
{
	check(ContextEffectData.StaticMeshComponent);
	check(Cast<UForceFeedbackEffect>(Effect));
	
	UForceFeedbackComponent* forceFeedbackComponent;
	AActor* owningActor = ContextEffectData.StaticMeshComponent->GetOwner();

	// Set ForceFeedback settings from ContextEffectData.CustomData
	FBSLForceFeedbackSettings forceFeedbackSettings;
	GetForceFeedbackSettings(ContextEffectData, forceFeedbackSettings);

	if (ContextEffectData.bAttached)
	{
		
		forceFeedbackComponent = UGameplayStatics::SpawnForceFeedbackAttached(
			CastChecked<UForceFeedbackEffect>(Effect),
			ContextEffectData.StaticMeshComponent,
			ContextEffectData.Socket,
			ContextEffectData.LocationOffset,
			ContextEffectData.RotationOffset,
			forceFeedbackSettings.AttachLocation,
			forceFeedbackSettings.bStopWhenAttachedToDestroyed,
			forceFeedbackSettings.bLooping,
			forceFeedbackSettings.IntensityMultiplier,
			forceFeedbackSettings.StartTime,
			forceFeedbackSettings.AttenuationSettings,
			forceFeedbackSettings.bAutoDestroy);
	}
	else
	{
		forceFeedbackComponent = UGameplayStatics::SpawnForceFeedbackAtLocation(
			owningActor,
			CastChecked<UForceFeedbackEffect>(Effect),
			ContextEffectData.LocationOffset,
			ContextEffectData.RotationOffset,
			forceFeedbackSettings.bLooping,
			forceFeedbackSettings.IntensityMultiplier,
			forceFeedbackSettings.StartTime,
			forceFeedbackSettings.AttenuationSettings,
			forceFeedbackSettings.bAutoDestroy);
	}

	// Tag spawned effect for future access
	if (!ContextEffectData.LoadedEffectTag.IsNone())
	{
		LoadedEffectTags.AddUnique(ContextEffectData.LoadedEffectTag);
	}

	// Apply all the system parameters to component
	ApplyForceFeedbackComponentParameters(forceFeedbackComponent, ContextEffectData);

	EffectComponent = forceFeedbackComponent;
	
	return true;
}

bool UBSLEffect_ForceFeedback::DestroyEffect()
{
	if (EffectComponent)
	{
		LoadedEffectTags.Empty();
		EffectComponent->Deactivate();
		return true;
	}
	return false;
}

void UBSLEffect_ForceFeedback::GetForceFeedbackSettings(const FBSLContextEffectData& ContextEffectData,
	FBSLForceFeedbackSettings& OutforceFeedbackSettings)
{
	for (const auto& data : ContextEffectData.CustomData)
	{
		if (const FBSLForceFeedbackSettings* forceFeedbackSettings = data.GetPtr<FBSLForceFeedbackSettings>())
		{
			OutforceFeedbackSettings = *forceFeedbackSettings;
#if WITH_EDITORONLY_DATA
			WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set forceFeedbackSettings for Effect %s on %s"), *GetClientServerContextString(),
				*ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
		}
	}
}

void UBSLEffect_ForceFeedback::ApplyForceFeedbackComponentParameters(UForceFeedbackComponent* ForceFeedbackComponent, const FBSLContextEffectData& ContextEffectData) const
{
	for (const auto& param : EffectDefinition->EffectParameters)
	{
		
		if (FBSLEffectParameterFloats::StaticStruct() == param.GetScriptStruct())
		{
			const FBSLEffectParameterFloats& floats = param.Get<FBSLEffectParameterFloats>();
			for (const auto& floatParam : floats.FloatParams)
			{
				//ForceFeedbackComponent->SetVariableFloat(floatParam.Key, floatParam.Value);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set insensity parameter name: %s value: %f for Effect %s on %s"), *GetClientServerContextString(),
					*floatParam.Key.ToString(), floatParam.Value, *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}
		}
		
		if (FBSLEffectParameterBools::StaticStruct() == param.GetScriptStruct())
		{
			FBSLEffectParameterBools bools = param.Get<FBSLEffectParameterBools>();
			for (const auto& boolParam : bools.BoolParams)
			{
				//ForceFeedbackComponent->SetVariableBool(boolParam.Key, boolParam.Value);
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set some other parameter name: %s value: %hhd for Effect %s on %s"), *GetClientServerContextString(),
					*boolParam.Key.ToString(), boolParam.Value, *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			}			
		}
	}
}
