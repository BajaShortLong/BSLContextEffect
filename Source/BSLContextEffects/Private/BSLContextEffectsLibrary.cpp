// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLContextEffectsLibrary.h"

#include "BSLGameplayTags.h"
#include "BSLEffect/BSLEffectData.h"
#include "Engine/AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BSLContextEffectsLibrary)

void UBSLContextEffectsLibrary::GetEffects(const FBSLContextEffectData& ContextEffectData, TArray<UBSLLoadedEffect*>& OutEffects)
{
	// Make sure Effect is valid and Library is loaded
	if (ContextEffectData.EffectTag.IsValid() && ContextEffectData.Contexts.IsValid() && EffectsLoadState == EBSLContextEffectsLibraryLoadState::Loaded)
	{		
		// Loop through Context Effects
		for (const FBSLActiveContextEffect& contextEffect : ActiveContextEffects)
		{
			// Matched EffectTag
			if (ContextEffectData.EffectTag.MatchesTagExact(contextEffect.EffectTag))
			{
				switch (ContextEffectData.ContextMatchType)
				{
					case EEffectsContextMatchType::FirstMatch:
					{
						// Get first EffectTag effects which match the context
						if (ContextEffectData.Contexts.HasAnyExact(contextEffect.Context)
							&& (contextEffect.Context.IsEmpty() == ContextEffectData.Contexts.IsEmpty()))
						{
							// Get all matching effects
							OutEffects.Append(contextEffect.Effects);
							return;
						}
					}
					case EEffectsContextMatchType::ExactMatch:
					{
						// Get first EffectTag effects which match the context
						if (ContextEffectData.Contexts.HasAllExact(contextEffect.Context)
							&& (contextEffect.Context.IsEmpty() == ContextEffectData.Contexts.IsEmpty()))
						{
							// Get all matching effects
							OutEffects.Append(contextEffect.Effects);
							return;
						}
					}
				}
			}
		}
	}
}

void UBSLContextEffectsLibrary::LoadEffects()
{
	// Load Effects into Library if not currently loading or loaded
	if (EffectsLoadState == EBSLContextEffectsLibraryLoadState::Unloaded)
	{
		// Set load state to loading
		EffectsLoadState = EBSLContextEffectsLibraryLoadState::Loading;
		
		// Clear out any old Active Effects
		ActiveContextEffects.Empty();

		// Call internal loading function
		LoadEffectsInternal();
	}
}

void UBSLContextEffectsLibrary::LoadEffectsInternal()
{	
	// Prepare Loaded Context Effects Array
	TArray<UBSLLoadedEffect*> LoadedContextEffectsArray;
	TArray<FPrimaryAssetId> AssetsToLoad;

	// Loop through Context Effects
	for (const FBSLContextEffect& contextEffect : ContextEffects)
	{
		// Make sure Tags are Valid
		if (contextEffect.EffectTag.IsValid() && contextEffect.Context.IsValid())
		{
			TArray<FSoftObjectPath> effectPaths;
			for (const auto& effectDefinition : contextEffect.Effects)
			{
				effectPaths.Add(effectDefinition->EffectAsset);
			}
			
			FStreamableDelegate delegate = FStreamableDelegate::CreateUObject(this, &ThisClass::ContextEffectLoadingComplete, contextEffect);
			if (UAssetManager* manager = UAssetManager::GetIfInitialized())
			{
				manager->LoadAssetList(effectPaths, delegate);
			}
		}
	}
}

void UBSLContextEffectsLibrary::ContextEffectLoadingComplete(FBSLContextEffect ContextEffect)
{
	// Create new Active Context Effect
	FBSLActiveContextEffect newContextEffect = FBSLActiveContextEffect(ContextEffect.EffectTag, ContextEffect.Context);
	
	for (const auto& effectDefinition : ContextEffect.Effects)
	{
		UBSLLoadedEffect* loadedEffect = effectDefinition->MakeLoadedEffect();
		check(loadedEffect->Effect);

		newContextEffect.Effects.Add(loadedEffect);
	}

	ActiveContextEffects.Add(newContextEffect);

	if (CheckEffectLoadingComplete())
	{
		ContextEffectLibraryLoadingComplete();
	}
}

bool UBSLContextEffectsLibrary::CheckEffectLoadingComplete() const
{
	if (ActiveContextEffects.Num() == ContextEffects.Num())
	{
		return true;
	}
	return false;
}

void UBSLContextEffectsLibrary::ContextEffectLibraryLoadingComplete()
{
	EffectsLoadState = EBSLContextEffectsLibraryLoadState::Loaded;
	/*LibraryLoadingComplete.ExecuteIfBound(this);*/
}

EBSLContextEffectsLibraryLoadState UBSLContextEffectsLibrary::GetContextEffectsLibraryLoadState() const
{
	return EffectsLoadState;
}
