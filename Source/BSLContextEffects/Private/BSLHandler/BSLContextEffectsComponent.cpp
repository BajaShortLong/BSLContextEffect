// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLHandler/BSLContextEffectsComponent.h"

#include "BSLContextEffectsLogging.h"
#include "BSLHandler/BSLContextEffectsSubsystem.h"
#include "BSLHandler/BSLEffectModifiers/BSLEffectModifierBase.h"


UBSLContextEffectsComponent::UBSLContextEffectsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void UBSLContextEffectsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld() && GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		// Don't setup on dedicated server
		return;
	}

	CurrentContexts.AppendTags(DefaultEffectContexts);
	CurrentContextEffectsLibraries = DefaultContextEffectsLibraries;

	// On Begin Play, Load and Add Context Effects pairings
	if (const UWorld* World = GetWorld())
	{
		if (UBSLContextEffectsSubsystem* BeansContextEffectsSubsystem = World->GetSubsystem<UBSLContextEffectsSubsystem>())
		{
			BeansContextEffectsSubsystem->LoadAndAddContextEffectsLibraries(GetOwner(), CurrentContextEffectsLibraries);
		}
	}

	RegisterRoutingRules();
	//RegisterEffectModifiers();
}

void UBSLContextEffectsComponent::HandleStartEffect_Implementation(FBSLContextEffectData& EffectData)
{
#if WITH_EDITORONLY_DATA
	if (GetWorld() && GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		UE_LOG(LogBSLContextEffects, Warning, TEXT("ContextEffect started on dedicated server! Event tag: %s"),
			*EffectData.EffectTag.ToString());
		return;
	}
#endif
	
	// Inject Contexts
	ModifyEffect(EffectData);
	
	// Add any current contexts
	EffectData.Contexts.AppendTags(CurrentContexts);

	// Route to external handlers
	RouteEffects(EffectData);
	
	// Default to have Subsystem handle effect
	TArray<UBSLLoadedEffect*> effectsToAdd;
	for (UBSLLoadedEffect* effect : ActiveContextEffects)
	{
		if (effect->EffectComponent)
		{
			effectsToAdd.Add(effect);
		}
	}
	
	if (const UWorld* World = GetWorld())
	{
		// Get Subsystem
		if (UBSLContextEffectsSubsystem* contextEffectsSubsystem = World->GetSubsystem<UBSLContextEffectsSubsystem>())
		{
			TArray<UBSLLoadedEffect*> spawnedEffects;

#if WITH_EDITORONLY_DATA
			if (!DebugTags.IsEmpty() && DebugTags.HasTag(EffectData.EffectTag))
			{
				WriteDebugMessage(EffectData, FString::Printf(TEXT("%s: Sending %s spawn request to ContextEffects subsystem for %s"), *GetClientServerContextString(),
					*EffectData.EffectTag.ToString(), *GetOwner()->GetName()));
			}
#endif
			
			// Spawn effects
			contextEffectsSubsystem->SpawnContextEffects(GetOwner(), EffectData, spawnedEffects);

			// Append resultant effects
			effectsToAdd.Append(spawnedEffects);
		}
	}

	// Append Active Audio Components
	ActiveContextEffects.Empty();
	ActiveContextEffects.Append(effectsToAdd);
}

void UBSLContextEffectsComponent::HandleEndEffect_Implementation(FName LoadedEffectTag)
{
#if WITH_EDITORONLY_DATA
	if (GetWorld() && GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		UE_LOG(LogBSLContextEffects, Warning, TEXT("ContextEffect ended on dedicated server! LoadedEffectTag: %s"),
			*LoadedEffectTag.ToString());
		return;
	}
#endif
	
	if (ActiveContextEffects.Num() > 0)
	{
		// Iterate all active effects because we could have 
		for (UBSLLoadedEffect* effect : ActiveContextEffects)
		{
			if (effect->EffectHasTag(LoadedEffectTag))
			{
				// Defer to Effect to destroy itself
				effect->DestroyEffect();
			}
		}
	}
}

FGameplayTagContainer UBSLContextEffectsComponent::GetDebugTags_Implementation()
{
#if WITH_EDITORONLY_DATA
	return DebugTags;
#else
	return FGameplayTagContainer::EmptyContainer;
#endif
}

void UBSLContextEffectsComponent::UpdateEffectContexts(FGameplayTagContainer NewEffectContexts)
{
	// Reset and update
	CurrentContexts.Reset(NewEffectContexts.Num());
	CurrentContexts.AppendTags(NewEffectContexts);
}

void UBSLContextEffectsComponent::UpdateLibraries(TSet<TSoftObjectPtr<UBSLContextEffectsLibrary>> NewContextEffectsLibraries)
{
	// Clear out existing Effects
	CurrentContextEffectsLibraries = NewContextEffectsLibraries;

	// Get World
	if (const UWorld* World = GetWorld())
	{
		// Get Subsystem
		if (UBSLContextEffectsSubsystem* BeansContextEffectsSubsystem = World->GetSubsystem<UBSLContextEffectsSubsystem>())
		{
			// Load and Add Libraries to Subsystem                  
			BeansContextEffectsSubsystem->LoadAndAddContextEffectsLibraries(GetOwner(), CurrentContextEffectsLibraries);
		}
	}
}

void UBSLContextEffectsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// On End PLay, remove unnecessary context effects pairings
	if (const UWorld* World = GetWorld())
	{
		if (UBSLContextEffectsSubsystem* BeansContextEffectsSubsystem = World->GetSubsystem<UBSLContextEffectsSubsystem>())
		{
			BeansContextEffectsSubsystem->UnloadAndRemoveContextEffectsLibraries(GetOwner());
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UBSLContextEffectsComponent::RegisterRoutingRules()
{
	// Load rules from datatable
	check(RoutingRulesTable)
	if (RoutingRulesTable)
	{
		RoutingRulesTable->GetAllRows("", EffectRoutingRules);
	}

	// Init our RuleCurrentHandlerMap
	for (const auto& rule : EffectRoutingRules)
	{
		for (const auto& tag : rule->MatchTags)
		{
			RuleCurrentHandlerMap.Add(tag, nullptr);
		}
	}

	UE_LOG(LogBSLContextEffects, Verbose, TEXT("%s: ContextEffectRules registered for %s"), *GetClientServerContextString(),
		*GetOwner()->GetName());
}

void UBSLContextEffectsComponent::RouteEffects(FBSLContextEffectData& EffectData)
{
	for (const auto& rule : EffectRoutingRules)
	{
		if (rule->MatchTags.HasTag(EffectData.EffectTag))
		{
			if (!IsValid(RuleCurrentHandlerMap[EffectData.EffectTag]))
			{
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.Owner = GetOwner();

				RuleCurrentHandlerMap[EffectData.EffectTag] = GetWorld()->SpawnActor<AActor>(rule->EffectHandlerClass,
					FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
				
				RuleCurrentHandlerMap[EffectData.EffectTag]->AttachToComponent(EffectData.StaticMeshComponent,
					FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative,EAttachmentRule::KeepRelative, true), FName("None"));

#if WITH_EDITORONLY_DATA
				if (!DebugTags.IsEmpty())
				{
					WriteDebugMessage(EffectData, FString::Printf(TEXT("%s: ContextEffect actor %s spawned for %s"), *GetClientServerContextString(),
						*RuleCurrentHandlerMap[EffectData.EffectTag]->GetName(), *GetOwner()->GetName()));
				}
#endif
			}

			// Call HandleStartEffectCopy so destination can modify EffectData as desired but won't affect other processing
			check(RuleCurrentHandlerMap[EffectData.EffectTag]->Implements<UBSLHandlerInterface>());
			Execute_HandleStartEffectRef(RuleCurrentHandlerMap[EffectData.EffectTag], EffectData);

#if WITH_EDITORONLY_DATA
			/*if (!DebugTags.IsEmpty())
			{
				WriteDebugMessage(EffectData, FString::Printf(TEXT("%s: HandleEffect called on %s for %s"), *GetClientServerContextString(),
					*RuleCurrentHandlerMap[EffectData.EffectTag]->GetName(), *GetOwner()->GetName()));
			}*/
#endif

			return; // exit after first match
		}
	}
}


void UBSLContextEffectsComponent::ModifyEffect(FBSLContextEffectData& ContextEffectData) const
{
	for (const auto& effectModifier : ContextEffectData.EffectModifiers)
	{
		if (!effectModifier)
		{
			continue;
		}
		
		effectModifier->ModifyEffect(ContextEffectData);
#if WITH_EDITORONLY_DATA
		if (!DebugTags.IsEmpty())
		{
			WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: EffectModifier %s modified effect %s on owner %s"),
				*GetClientServerContextString(), *effectModifier->GetName(), *ContextEffectData.EffectTag.ToString(), *GetOwner()->GetName()));
		}
#endif
	}
}
