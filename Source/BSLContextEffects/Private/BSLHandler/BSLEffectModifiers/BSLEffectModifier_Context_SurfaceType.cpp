// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_Context_SurfaceType.h"

#include "BSLContextEffectsLogging.h"
#include "BSLHandler/BSLContextEffectsComponent.h"
#include "BSLHandler/BSLContextEffectsSubsystem.h"


bool UBSLEffectModifier_Context_SurfaceType::ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const
{
	if (Super::ModifyEffect_Implementation(ContextEffectData))
	{
		FGameplayTag surfaceContextTag = GetPhysicalSurfaceContextTag(ContextEffectData);
		if (surfaceContextTag.MatchesTag(FGameplayTag::EmptyTag))
		{
			return false;
		}
		
		ContextEffectData.Contexts.AddTag(surfaceContextTag);

#if WITH_EDITORONLY_DATA
		WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Added surface context tag %s for effect %s on %s"), *GetClientServerContextString(),
			*surfaceContextTag.ToString(), *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif

		return true;
	}

	return false;
}

FGameplayTag UBSLEffectModifier_Context_SurfaceType::GetPhysicalSurfaceContextTag(const FBSLContextEffectData& ContextEffectData) const
{
	// Prep physical material
	TWeakObjectPtr<UPhysicalMaterial> physicalMaterialPtr;
	
	for (const auto& data : ContextEffectData.CustomData)
	{
		if (const FBSLHitResult* hitResult = data.GetPtr<FBSLHitResult>())
		{
			 physicalMaterialPtr = hitResult->HitResult.PhysMaterial;
#if WITH_EDITORONLY_DATA
			WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set TraceSettings for Effect %s on %s"), *GetClientServerContextString(),
				*ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			break;
		}
	}
	
	// Check if pointer is okay
	if (physicalMaterialPtr.IsValid())
	{
		// Get the Surface Type Pointer
		TEnumAsByte<EPhysicalSurface> PhysicalSurfaceType = physicalMaterialPtr->SurfaceType;

		// If Settings are valid
		if (const UBSLContextEffectsSettings* BSLContextEffectsSettings = GetDefault<UBSLContextEffectsSettings>())
		{
			// Convert Surface Type to known
			if (const FGameplayTag* SurfaceContextPtr = BSLContextEffectsSettings->SurfaceTypeToContextMap.Find(PhysicalSurfaceType))
			{
				return *SurfaceContextPtr;
			}
		}
	}

	return FGameplayTag::EmptyTag;
}
