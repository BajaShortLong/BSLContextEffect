// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLEmitter/AnimNotify_BSLContextEffects.h"

#include "BSLContextEffectsLogging.h"
#include "BSLHandler/BSLContextEffectsComponent.h"
#include "BSLHandler/BSLContextEffectsSubsystem.h"
#include "BSLHandler/BSLHandlerInterface.h"
#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_RelativeToMeshComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_BSLContextEffects)



UAnimNotify_BSLContextEffects::UAnimNotify_BSLContextEffects()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor::Purple;
#endif

	ContextEffectData.EffectModifiers.Add(NewObject<UBSLEffectModifier_RelativeToMeshComponent>());
}

FString UAnimNotify_BSLContextEffects::GetNotifyName_Implementation() const
{
	// If the Effect Tag is valid, pass the string name to the notify name
	if (ContextEffectData.EffectTag.IsValid())
	{
		return ContextEffectData.EffectTag.ToString();
	}

	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_BSLContextEffects::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
#if WITH_EDITORONLY_DATA
	if (GetWorld() && GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		UE_LOG(LogBSLContextEffects, Warning, TEXT("ContextEffect Notify called on dedicated server! Event tag: %s AnimAsset: %s"),
			*ContextEffectData.EffectTag.ToString(), *Animation->GetName());
		return;
	}
#endif
	
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		// Make sure both MeshComp and Owning Actor is valid
		if (AActor* owningActor = MeshComp->GetOwner())
		{
			FBSLContextEffectData newContextEffect = ContextEffectData;
			newContextEffect.StaticMeshComponent = MeshComp;
			
			// Add EffectModifier to set location/rotation if not attached
			/*if (!newContextEffect.bAttached)
			{
				newContextEffect.EffectModifiers.Add(NewObject<UBSLEffectModifier_RelativeToMeshComponent>(this));
			}*/
			
			if (UBSLContextEffectsComponent* contextEffectComponent = UBSLContextEffectsComponent::FindContextEffectsComponent(owningActor))
			{
				IBSLHandlerInterface::Execute_HandleStartEffect(contextEffectComponent, newContextEffect);
			}

#if WITH_EDITORONLY_DATA
			// This is for Anim Editor previewing, it is a deconstruction of the calls made by the Interface and the Subsystem.
			// Very possible this results in eventual OOM for effects that don't kill themselves.
			if (bPreviewInEditor)
			{
				UWorld* World = owningActor->GetWorld();

				// Get the world, make sure it's an Editor Preview World
				if (World && World->WorldType == EWorldType::EditorPreview)
				{
					// Add Preview contexts if necessary
					newContextEffect.Contexts.AppendTags(PreviewProperties.PreviewContexts);

					// Convert given Surface Type to Context and Add it to the Contexts for this Preview
					if (PreviewProperties.bPreviewPhysicalSurfaceAsContext)
					{
						TEnumAsByte<EPhysicalSurface> PhysicalSurfaceType = PreviewProperties.PreviewPhysicalSurface;

						if (const UBSLContextEffectsSettings* BSLContextEffectsSettings = GetDefault<UBSLContextEffectsSettings>())
						{
							if (const FGameplayTag* SurfaceContextPtr = BSLContextEffectsSettings->SurfaceTypeToContextMap.Find(PhysicalSurfaceType))
							{
								FGameplayTag SurfaceContext = *SurfaceContextPtr;

								newContextEffect.Contexts.AddTag(SurfaceContext);
							}
						}
					}

					// Libraries are soft referenced, so you will want to try to load them now
					if (UObject* EffectsLibrariesObj = PreviewProperties.PreviewContextEffectsLibrary.TryLoad())
					{
						// Check if it is in fact a UBSLContextEffectLibrary type
						if (UBSLContextEffectsLibrary* effectLibrary = Cast<UBSLContextEffectsLibrary>(EffectsLibrariesObj))
						{
							// Set up local list of Sounds and Niagara Systems
							TArray<UBSLLoadedEffect*> effects;
							
							// Attempt to load the Effect Library content (will cache in Transient data on the Effect Library Asset)
							effectLibrary->LoadEffects();

							// If the Effect Library is valid and marked as Loaded, Get Effects from it
							if (effectLibrary && effectLibrary->GetContextEffectsLibraryLoadState() == EBSLContextEffectsLibraryLoadState::Loaded)
							{
								effectLibrary->GetEffects(newContextEffect, effects);
							}

							// Spawn all matched effects
							for (const auto& effect : effects)
							{
								// Set the effect component. This could be nullptr if the effect doesn't return a component (fire and forget)
								effect->SpawnEffect(newContextEffect);

							}
						}
					}
						
				}
			}
#endif
		}
	}
}

#if WITH_EDITOR
void UAnimNotify_BSLContextEffects::ValidateAssociatedAssets()
{
	Super::ValidateAssociatedAssets();
}

void UAnimNotify_BSLContextEffects::SetParameters(FBSLContextEffectData InContextEffectData, bool InPreviewInEditor,
	FBSLContextEffectAnimNotifyPreviewSettings InPreviewSettings)
{
	ContextEffectData = InContextEffectData;
	bPreviewInEditor = InPreviewInEditor;
	PreviewProperties = InPreviewSettings;
}
#endif
