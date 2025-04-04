// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLHandler/BSLEffectModifiers/BSLEffectModifier_Trace.h"

#include "BSLContextEffectsLogging.h"
#include "BSLHandler/BSLContextEffectsComponent.h"

const FBSLContextTraceSettings FBSLContextTraceSettings::EmptySettings;

bool UBSLEffectModifier_Trace::ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const
{
	// We should probably have a valid static mesh
	if (!ContextEffectData.StaticMeshComponent)
	{
		UE_LOG(LogBSLContextEffects, Error, TEXT("%s: StaticMeshComponent is invalid for effect %s"),
			*GetClientServerContextString(), *ContextEffectData.EffectTag.ToString());
		return false;
	}
	
	const FBSLContextTraceSettings& traceSettings = GetTraceSettings(ContextEffectData);
	
	FHitResult newHitResult;
	PerformTrace(ContextEffectData, traceSettings, newHitResult);

	SetContextEffectDataHitResult(newHitResult, ContextEffectData);
	
	return newHitResult.bBlockingHit;
}

const FBSLContextTraceSettings& UBSLEffectModifier_Trace::GetTraceSettings(
	const FBSLContextEffectData& ContextEffectData) const
{
	for (const auto& data : ContextEffectData.CustomData)
	{
		if (const FBSLContextTraceSettings* traceSettings = data.GetPtr<FBSLContextTraceSettings>())
		{
			
#if WITH_EDITORONLY_DATA
			WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set TraceSettings for Effect %s on %s"), *GetClientServerContextString(),
				*ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
			
			return *traceSettings;
		}
	}

#if WITH_EDITORONLY_DATA
	WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: No BSLContextTraceSettings found in CustomData for %s on %s, using default TraceSettings"),
		*GetClientServerContextString(), *ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif

	return FBSLContextTraceSettings::EmptySettings;
}

void UBSLEffectModifier_Trace::SetContextEffectDataHitResult(const FHitResult& InHitResult,
	FBSLContextEffectData& ContextEffectData) const
{
	FBSLHitResult hitResult;
	hitResult.HitResult = InHitResult;
	
	const FInstancedStruct hitResultStruct = FInstancedStruct::Make(hitResult);
	ContextEffectData.CustomData.Add(hitResultStruct);
}

void UBSLEffectModifier_Trace::PerformTrace(const FBSLContextEffectData& ContextEffectData,
	const FBSLContextTraceSettings& InTraceSettings, FHitResult& OutHitResult) const
{
	FCollisionQueryParams QueryParams;
	AActor* owningActor = ContextEffectData.StaticMeshComponent->GetOwner();
	if (InTraceSettings.bIgnoreActor)
	{
		QueryParams.AddIgnoredActor(owningActor);
	}
	QueryParams.bReturnPhysicalMaterial = InTraceSettings.bReturnPhysicalMaterial;
	
	FVector traceStart, traceEnd;

	// If attached to something, transform into local space
	if (ContextEffectData.bAttached)
	{
		traceStart = ContextEffectData.StaticMeshComponent->GetSocketTransform(ContextEffectData.Socket).TransformPosition(InTraceSettings.StartTraceLocationOffset);
		traceEnd = ContextEffectData.StaticMeshComponent->GetSocketTransform(ContextEffectData.Socket).TransformPosition(InTraceSettings.EndTraceLocationOffset);
	} else
	{
		traceStart = owningActor->GetTransform().TransformPosition(InTraceSettings.StartTraceLocationOffset);
		traceEnd = owningActor->GetTransform().TransformPosition(InTraceSettings.EndTraceLocationOffset);
	}

	// Make sure World is valid
	if (UWorld* World = owningActor->GetWorld())
	{
		// Call Line Trace, Pass in relevant properties
		World->LineTraceSingleByChannel(OutHitResult, traceStart, traceEnd,
			InTraceSettings.TraceChannel, QueryParams, FCollisionResponseParams::DefaultResponseParam);

#if WITH_EDITORONLY_DATA
		UBSLContextEffectsComponent* contextEffectsComponent = UBSLContextEffectsComponent::FindContextEffectsComponent(owningActor);
		if (contextEffectsComponent && contextEffectsComponent->DebugTags.HasTag(ContextEffectData.EffectTag))
		{
			DrawDebugDirectionalArrow(World, traceStart, traceEnd, 5, FColor::Red, false, 5, 0, 1);
			if (OutHitResult.bBlockingHit)
			{
				DrawDebugSphere(World, OutHitResult.ImpactPoint, 5, 5, FColor::Green, false, 5);
			}

			if (!OutHitResult.bBlockingHit)
			{
				UE_LOG(LogBSLContextEffects, Warning, TEXT("Trace missed!"));
			}
		}
#endif
	}
}
