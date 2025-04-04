// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLEmitter/AnimNotifyState_BSLContextEffects.h"

#include "BSLContextEffectsLogging.h"
#include "BSLHandler/BSLContextEffectsComponent.h"
#include "BSLHandler/BSLHandlerInterface.h"
#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_RelativeToMeshComponent.h"


UAnimNotifyState_BSLContextEffects::UAnimNotifyState_BSLContextEffects()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor::Yellow;
#endif

	ContextEffectData.EffectModifiers.Add(NewObject<UBSLEffectModifier_RelativeToMeshComponent>());
}

FString UAnimNotifyState_BSLContextEffects::GetNotifyName_Implementation() const
{
	// If the Effect Tag is valid, pass the string name to the notify name
	if (ContextEffectData.EffectTag.IsValid())
	{
		return ContextEffectData.EffectTag.ToString();
	}

	return Super::GetNotifyName_Implementation();
}

void UAnimNotifyState_BSLContextEffects::NotifyBegin(class USkeletalMeshComponent* MeshComp,
	class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
#if WITH_EDITORONLY_DATA
	if (GetWorld() && GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		UE_LOG(LogBSLContextEffects, Warning, TEXT("ContextEffect NotifyState called on dedicated server! Event tag: %s AnimAsset: %s"),
			*ContextEffectData.EffectTag.ToString(), *Animation->GetName());
		return;
	}
#endif
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (MeshComp)
	{
		// Make sure both MeshComp and Owning Actor is valid
		if (AActor* owningActor = MeshComp->GetOwner())
		{
			FBSLContextEffectData newContextEffect = ContextEffectData;
			newContextEffect.StaticMeshComponent = MeshComp;
			newContextEffect.LoadedEffectTag = GetLoadedEffectTag();

			// Add EffectModifier to set location/rotation if not attached
			/*if (!newContextEffect.bAttached)
			{
				newContextEffect.EffectModifiers.Add(NewObject<UBSLEffectModifier_RelativeToMeshComponent>(this));
			}*/
			
			if (UBSLContextEffectsComponent* contextEffectComponent = UBSLContextEffectsComponent::FindContextEffectsComponent(owningActor))
			{
				IBSLHandlerInterface::Execute_HandleStartEffect(contextEffectComponent, newContextEffect);
			}
		}
	}
}

void UAnimNotifyState_BSLContextEffects::NotifyEnd(class USkeletalMeshComponent* MeshComp,
	class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		// Make sure both MeshComp and Owning Actor is valid
		if (AActor* owningActor = MeshComp->GetOwner())
		{
			if (UBSLContextEffectsComponent* contextEffectComponent = UBSLContextEffectsComponent::FindContextEffectsComponent(owningActor))
			{
				IBSLHandlerInterface::Execute_HandleEndEffect(contextEffectComponent, GetLoadedEffectTag());
			}
		}
	}
}
