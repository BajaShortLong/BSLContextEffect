// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_Context_Children.h"

#include "BSLHandler/BSLContextEffectsComponent.h"

bool UBSLEffectModifier_Context_Children::ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const
{
	if (ContextEffectData.StaticMeshComponent)
	{
		if (AActor* owner = ContextEffectData.StaticMeshComponent->GetOwner())
		{
			if (UBSLContextEffectsComponent* contextEffectsComponent = UBSLContextEffectsComponent::FindContextEffectsComponent(owner))
			{
				// Get all the children tags of the ParentTag that was passed in
				FGameplayTagContainer childrenTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(ParentTag);

				// Get tags in the DefaultEffectContext which match the children tags
				FGameplayTagContainer tagsToAdd = contextEffectsComponent->CurrentContexts.FilterExact(childrenTags);

				if (tagsToAdd.IsEmpty())
				{
					ContextEffectData.Contexts.AddTag(DefaultTag);
				}
				else
				{
					ContextEffectData.Contexts.AppendTags(tagsToAdd);
				}

				return true;
			}
		}
	}
	
	return false;
}
