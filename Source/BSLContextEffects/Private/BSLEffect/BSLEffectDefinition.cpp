// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLContextEffects/Public/BSLEffect/BSLEffectDefinition.h"

#include "BSLContextEffectsLogging.h"

TArray<UClass*> UBSLEffectDefinition::GetAllowedAssetClass_Implementation() const
{
	return TArray<UClass*>();
}

UBSLLoadedEffect* UBSLEffectDefinition::MakeLoadedEffect_Implementation()
{
	return nullptr;
}

bool UBSLLoadedEffect::SpawnEffect(const FBSLContextEffectData& ContextEffectData)
{
	UE_LOG(LogBSLContextEffects, Error, TEXT("Attempting to spawn effect from base LoadedEffect class!"));
	return false;
}

bool UBSLLoadedEffect::DestroyEffect()
{
	if (EffectComponent)
	{
		LoadedEffectTags.Empty();
		EffectComponent->DestroyComponent();
		return true;
	}
	return false;
}

bool UBSLLoadedEffect::EffectHasTag(FName Tag) const
{
	return (Tag != NAME_None) && LoadedEffectTags.Contains(Tag);
}
