// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLHandler/BSLContextEffectsSubsystem.h"

#include "BSLContextEffectsLogging.h"
#include "BSLEffect/BSLEffectData.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void UBSLContextEffectsSubsystem::SpawnContextEffects(const AActor* SpawningActor, FBSLContextEffectData ContextEffectData,
	TArray<UBSLLoadedEffect*>& OutEffects)
{
	// First determine if this Actor has any libraries
	if (UBSLContextEffectsLibrarySet** librariesSetPtr = ActiveActorEffectsMap.Find(SpawningActor))
	{
		if (UBSLContextEffectsLibrarySet* librariesSet = *librariesSetPtr)
		{
			TArray<UBSLLoadedEffect*> totalEffects;

			// Cycle through Effect Libraries
			for (UBSLContextEffectsLibrary* effectLibrary : librariesSet->BSLContextEffectsLibraries)
			{
				check(effectLibrary);
				
				// Check if the Effect Library is valid and data Loaded
				if (effectLibrary->GetContextEffectsLibraryLoadState() == EBSLContextEffectsLibraryLoadState::Loaded)
				{
					// Set up local list of Sounds and Niagara Systems
					TArray<UBSLLoadedEffect*> effects;
					
					// Get effects
					effectLibrary->GetEffects(ContextEffectData, effects);

					// Append to accumulating array
					totalEffects.Append(effects);

					continue;
				}
				
				// Library loading should be performed before the need to spawn effects otherwise we could miss initial spawn attempts
				UE_LOG(LogBSLContextEffects, Warning, TEXT("Attempted to spawn effects from a library that isn't loaded! Library: %s, LoadingState: %s"),
					*effectLibrary->GetName(), *UEnum::GetValueAsString(effectLibrary->GetContextEffectsLibraryLoadState()))

				// reluctantly load effects here
				effectLibrary->LoadEffects();
			}

			// Spawn all matched effects
			for (const auto& effect : totalEffects)
			{
				// Should probably just not add locally controlled only effects to actors map for non-locally controlled actors
				const APawn* owningPawn = Cast<APawn>(SpawningActor);
				if (owningPawn && effect->EffectDefinition->bLocallyControlledOnly && !owningPawn->IsLocallyControlled())
				{
					continue;
				}

				// Create another effect because we don't want to be spawning from the effect loaded from the library
				UBSLLoadedEffect* newEffect = effect->EffectDefinition->MakeLoadedEffect();
				newEffect->SpawnEffect(ContextEffectData);
				
				OutEffects.Add(newEffect);
				
	#if WITH_EDITORONLY_DATA
				WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Spawning %s FX for %s from effect tag %s"), *GetClientServerContextString(),
					*effect->GetName(), *SpawningActor->GetName(), *ContextEffectData.EffectTag.ToString()));
	#endif
			}
		}
	}
}

void UBSLContextEffectsSubsystem::LoadAndAddContextEffectsLibraries(AActor* OwningActor,
	TSet<TSoftObjectPtr<UBSLContextEffectsLibrary>> ContextEffectsLibraries)
{
	// Early out if Owning Actor is invalid or if the associated Libraries is 0 (or less)
	if (OwningActor == nullptr || ContextEffectsLibraries.Num() <= 0)
	{
		return;
	}

	// Cycle through Libraries getting Soft Obj Refs and async load them
	for (const TSoftObjectPtr<UBSLContextEffectsLibrary>& contextEffectSoftObj : ContextEffectsLibraries)
	{
		FStreamableDelegate delegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnContextEffectsLibraryLoaded, contextEffectSoftObj, OwningActor);
		if (UAssetManager* manager = UAssetManager::GetIfInitialized())
		{
			TArray<FSoftObjectPath> softObjectPath;
			softObjectPath.Add(contextEffectSoftObj.ToSoftObjectPath());
			manager->LoadAssetList(softObjectPath, delegate);
		}
	}
}

void UBSLContextEffectsSubsystem::OnContextEffectsLibraryLoaded(
	TSoftObjectPtr<UBSLContextEffectsLibrary> LoadedContextEffectsLibrary, AActor* OwningActor)
{
	UBSLContextEffectsLibrary* library = Cast<UBSLContextEffectsLibrary>(LoadedContextEffectsLibrary.ToSoftObjectPath().ResolveObject());
	check(library);
	if (library != nullptr)
	{
		// Load effect assets
		library->LoadEffects();
		
		if (UBSLContextEffectsLibrarySet** librariesSetPtr = ActiveActorEffectsMap.Find(OwningActor))
		{
			if (UBSLContextEffectsLibrarySet* librarySet = *librariesSetPtr)
			{
				librarySet->BSLContextEffectsLibraries.Add(library);
			} 
		}
		else
		{
			// Init a new set and add library
			TSet<UBSLContextEffectsLibrary*> newSet;
			newSet.Add(library);
			// Create new LibrarySet and add new set to it
			UBSLContextEffectsLibrarySet* librarySet = NewObject<UBSLContextEffectsLibrarySet>();
			librarySet->BSLContextEffectsLibraries = newSet;
			// Add set to ActiveActor map
			ActiveActorEffectsMap.Add(OwningActor, librarySet);
		}
	}	
}

void UBSLContextEffectsSubsystem::UnloadAndRemoveContextEffectsLibraries(AActor* OwningActor)
{
	if (OwningActor == nullptr)
	{
		return;
	}

	// Remove ref from Active Actor/Effects Set Map
	ActiveActorEffectsMap.Remove(OwningActor);
}
