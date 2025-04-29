// Fill out your copyright notice in the Description page of Project Settings.

#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_InitialVelocity.h"

bool UBSLEffectModifier_InitialVelocity::ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const
{
	if (ContextEffectData.StaticMeshComponent)
	{
		if (AActor* owningActor = ContextEffectData.StaticMeshComponent->GetOwner())
		{
			FBSLVelocity initVelocity = FBSLVelocity(owningActor->GetVelocity());
			FInstancedStruct initVelocityWrapper = FInstancedStruct::Make(initVelocity);
			ContextEffectData.CustomData.Add(initVelocityWrapper);

			return true;
		}
	}
	
	return false;
}
