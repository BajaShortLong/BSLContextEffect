// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLEmitter/BSLStateChangeComponent.h"

#include "BSLContextEffectsLogging.h"
#include "BSLHandler/BSLContextEffectsComponent.h"
#include "BSLHandler/BSLHandlerInterface.h"
#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_RelativeToMeshComponent.h"


// Sets default values for this component's properties
UBSLStateChangeComponent::UBSLStateChangeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBSLStateChangeComponent::BindToSourceOfTruth_Implementation()
{
	UE_LOG(LogBSLContextEffects, Error, TEXT("%s: BindToSourceOfTruth() must be overridden in BSLStateChangeComponent"), *GetClientServerContextString());
}

void UBSLStateChangeComponent::SetCacheables_Implementation()
{
	MeshComponent = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	ContextEffectsComponent = UBSLContextEffectsComponent::FindContextEffectsComponent(GetOwner());

	check(ContextEffectsComponent);
	check(MeshComponent);
}

// Called when the game starts
void UBSLStateChangeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld() && GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		// Don't setup on dedicated server
		return;
	}
	
	RegisterStateEffectRules();
	SetCacheables();
	BindToSourceOfTruth();
}

void UBSLStateChangeComponent::RegisterStateEffectRules()
{
	// Load rules from datatable
	check(StateEffectRuleTable)
	if (StateEffectRuleTable)
	{
		StateEffectRuleTable->GetAllRows("", StateEffectRules);

		for (int idx = 0; idx < StateEffectRules.Num(); idx++)
		{
			StateAnimEffectMap.Add(StateEffectRules[idx]->StateTag, idx);

			// Add our state effects to runtime containers
			if (StateEffectRules[idx]->bOnAdded)
			{
				RegisteredTagsForAdded.AddTag(StateEffectRules[idx]->StateTag);
			}
			if (StateEffectRules[idx]->bOnRemoved || StateEffectRules[idx]->bDestroyOnTagRemoved)
			{
				RegisteredTagsForRemoved.AddTag(StateEffectRules[idx]->StateTag);
			}
		}
	}
}

void UBSLStateChangeComponent::HandleActiveTagsChanged(FGameplayTagContainer AddedTags, FGameplayTagContainer RemovedTags, const FGameplayTagContainer CurrentTags)
{
	if (RegisteredTagsForAdded.HasAny(AddedTags))
	{
		for (const auto& tag : AddedTags)
		{
			if (StateAnimEffectMap.Contains(tag))
			{
				FBSLContextEffectData effectData = PrepareEffectData(tag);
				IBSLHandlerInterface::Execute_HandleStartEffect(ContextEffectsComponent, effectData);

#if WITH_EDITORONLY_DATA
				if (ContextEffectsComponent && !ContextEffectsComponent->DebugTags.IsEmpty())
				{
					WriteDebugMessage(effectData, FString::Printf(TEXT("%s: Execute effect start for state: %s with effect: %s on %s"), *GetClientServerContextString(),
						*tag.ToString(), *effectData.EffectTag.ToString(), *GetOwner()->GetName()));
				}
#endif
			}
		}
	}

	if (RegisteredTagsForRemoved.HasAny(RemovedTags))
	{
		for (const auto& tag : RemovedTags)
		{
			if (StateAnimEffectMap.Contains(tag))
			{
				// Check and continue if we're just destroying effects from AddedTags
				if (StateEffectRules[StateAnimEffectMap[tag]]->bDestroyOnTagRemoved)
				{
					// Check if all instances of tag should be removed
					if (StateEffectRules[StateAnimEffectMap[tag]]->bWaitForAllInstances && CurrentTags.HasTag(tag))
					{
						continue;
					}
					
					IBSLHandlerInterface::Execute_HandleEndEffect(ContextEffectsComponent, GetLoadedEffectTag(tag));

#if WITH_EDITORONLY_DATA
					if (ContextEffectsComponent && !ContextEffectsComponent->DebugTags.IsEmpty())
					{
						const FBSLContextEffectData& effectData = StateEffectRules[StateAnimEffectMap[tag]]->AddedEffectData;
						WriteDebugMessage(effectData, FString::Printf(TEXT("%s: Destroy effects %s on removal of state: %s on %s"), *GetClientServerContextString(),
							*effectData.EffectTag.ToString(), *tag.ToString(), *GetOwner()->GetName()));
					}
#endif
					
					continue;
				}
				
				FBSLContextEffectData effectData = PrepareEffectData(tag);
				IBSLHandlerInterface::Execute_HandleStartEffect(ContextEffectsComponent, effectData);

#if WITH_EDITORONLY_DATA
				if (ContextEffectsComponent && !ContextEffectsComponent->DebugTags.IsEmpty())
				{
					WriteDebugMessage(effectData, FString::Printf(TEXT("%s: Execute effect stop for state: %s with effect: %s on %s"), *GetClientServerContextString(),
						*tag.ToString(), *effectData.EffectTag.ToString(), *GetOwner()->GetName()));
				}
#endif

			}
		}
	}
}

FBSLContextEffectData UBSLStateChangeComponent::PrepareEffectData(FGameplayTag EffectTag)
{
	FBSLContextEffectData effectData = StateEffectRules[StateAnimEffectMap[EffectTag]]->AddedEffectData;
	effectData.StaticMeshComponent = MeshComponent;
	effectData.LoadedEffectTag = GetLoadedEffectTag(EffectTag);
	//ProcessEffectTransform(effectData);
	if (!effectData.bAttached)
	{
		effectData.EffectModifiers.Add(NewObject<UBSLEffectModifier_RelativeToMeshComponent>(this));
	}

	return effectData;
}

void UBSLStateChangeComponent::ProcessEffectTransform(FBSLContextEffectData& EffectData)
{
	if (!EffectData.bAttached && !EffectData.Socket.IsNone())
	{
		EffectData.LocationOffset = EffectData.StaticMeshComponent->GetSocketTransform(EffectData.Socket).TransformPosition(EffectData.LocationOffset);

		if (EffectData.bUseBoneRotation)
		{
			EffectData.RotationOffset = EffectData.StaticMeshComponent->GetSocketTransform(EffectData.Socket).TransformRotation(EffectData.RotationOffset.Quaternion()).Rotator();
		}
	}
}

FName UBSLStateChangeComponent::GetLoadedEffectTag(FGameplayTag Tag) const
{
	return FName(FString::Printf(TEXT("StateEffect_%s"), *Tag.ToString()));
}

