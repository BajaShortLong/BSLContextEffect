// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_RelativeToMeshComponent.h"

#include "BSLContextEffectsLogging.h"

bool UBSLEffectModifier_RelativeToMeshComponent::ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const
{

	if (!ContextEffectData.StaticMeshComponent)
	{
		UE_LOG(LogBSLContextEffects, Error, TEXT("StaticMeshComponent is invalid! Can't set this effect's transform relative to MeshComponent EffectTag: %s"),
			*ContextEffectData.EffectTag.ToString());

		return false;
	}
	
	if (!ContextEffectData.bAttached && !ContextEffectData.Socket.IsNone())
	{
		ContextEffectData.LocationOffset = ContextEffectData.StaticMeshComponent->GetSocketTransform(ContextEffectData.Socket).TransformPosition(ContextEffectData.LocationOffset);

		if (ContextEffectData.bUseBoneRotation)
		{
			ContextEffectData.RotationOffset = ContextEffectData.StaticMeshComponent->GetSocketTransform(ContextEffectData.Socket).TransformRotation(ContextEffectData.RotationOffset.Quaternion()).Rotator();
		}
	}
	
	return true;
}
