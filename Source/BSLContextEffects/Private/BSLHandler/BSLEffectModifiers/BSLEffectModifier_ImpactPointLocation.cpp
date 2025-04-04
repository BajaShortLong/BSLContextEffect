// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_ImpactPointLocation.h"

#include "BSLContextEffectsLogging.h"
#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_Trace.h"


bool UBSLEffectModifier_ImpactPointLocation::ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const
{
	if (Super::ModifyEffect_Implementation(ContextEffectData))
	{
		for (const auto& data : ContextEffectData.CustomData)
		{
			if (const FBSLHitResult* hitResult = data.GetPtr<FBSLHitResult>())
			{
				ContextEffectData.LocationOffset += hitResult->HitResult.ImpactPoint;
				
#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set ImpactPoint to Location for effect %s on %s"), *GetClientServerContextString(),
					*ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
				
				return true;
			}
		}

		return false;
	}
	
	return false;
}
