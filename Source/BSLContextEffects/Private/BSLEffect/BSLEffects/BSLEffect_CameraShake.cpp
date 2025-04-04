// Fill out your copyright notice in the Description page of Project Settings.


#include "BSLEffect/BSLEffects/BSLEffect_CameraShake.h"

#include "BSLContextEffectsLogging.h"
#include "Camera/CameraModifier_CameraShake.h"
#include "Kismet/GameplayStatics.h"
#include "Shakes/DefaultCameraShakeBase.h"


UBSLEffectDefinition_CameraShake::UBSLEffectDefinition_CameraShake()
{
	bLocallyControlledOnly = true;
}

UBSLLoadedEffect* UBSLEffectDefinition_CameraShake::MakeLoadedEffect_Implementation()
{
	UBSLEffect_CameraShake* loadedEffect = NewObject<UBSLEffect_CameraShake>(GetOuter());
	
	// If we spawn a blueprint class we need to create the object the blueprint class is of
	UBlueprint* blueprintAsset = Cast<UBlueprint>(EffectAsset.ResolveObject());
	loadedEffect->Effect = NewObject<UDefaultCameraShakeBase>(GetOuter(), blueprintAsset->GeneratedClass);
	loadedEffect->EffectDefinition = this;
	
	return loadedEffect;
}

TArray<UClass*> UBSLEffectDefinition_CameraShake::GetAllowedAssetClass_Implementation() const
{
	// I tried a method to specify UBlueprint classes with ParentClass of UDefaultCameraShakeBase but couldn't get it to
	// work. I think GetAllowedClasses only works with C++ classes but haven't looked further

	TArray<UClass*> outClass;
	outClass.Add(UBlueprint::StaticClass());
	return outClass;
}

bool UBSLEffect_CameraShake::SpawnEffect(const FBSLContextEffectData& ContextEffectData)
{
	check(ContextEffectData.StaticMeshComponent);
	check(Cast<UDefaultCameraShakeBase>(Effect));

	/*if (!Effect)
	{
		return false;
	}*/

	UDefaultCameraShakeBase* cameraShake = Cast<UDefaultCameraShakeBase>(Effect);
	
	// This effect should only be called to spawn on locally controlled owning actor
#if WITH_EDITORONLY_DATA
	if (APawn* owningPawn = Cast<APawn>(ContextEffectData.StaticMeshComponent->GetOwner()))
	{
		check(owningPawn->IsLocallyControlled());
	}
#endif
	
	FBSLCameraShakeSettings cameraShakeSettings;
	GetCameraShakeSettings(ContextEffectData, cameraShakeSettings);
	const FAddCameraShakeParams Params = FAddCameraShakeParams(cameraShakeSettings.Scale, cameraShakeSettings.PlaySpace, cameraShakeSettings.UserPlaySpaceRot);

	if (APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(ContextEffectData.StaticMeshComponent, 0))
	{
		Effect = cameraManager->StartCameraShake(cameraShake->GetClass(), Params);
	}

	// Setup timer to tear this camera shake down if a duration is set
	FCameraShakeDuration shakeDuration;
	UDefaultCameraShakeBase::GetCameraShakeDuration(EffectDefinition->EffectAsset.ResolveObject()->StaticClass(), shakeDuration);
	if (shakeDuration.Get() > 0)
	{
		FTimerDelegate destroyTimerDelegate;
		destroyTimerDelegate.BindUFunction(this, "DestroyEffect");
		ContextEffectData.StaticMeshComponent->GetWorld()->GetTimerManager().SetTimer(
			DestructionTimer, destroyTimerDelegate, shakeDuration.Get() + 0.1f, false, -1);
	}

	// Tag spawned effect for future access
	if (!ContextEffectData.LoadedEffectTag.IsNone())
	{
		LoadedEffectTags.AddUnique(ContextEffectData.LoadedEffectTag);
	}
	
	return true;
}

bool UBSLEffect_CameraShake::DestroyEffect()
{
	if (APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetOuter(), 0))
	{
		cameraManager->StopCameraShake(Cast<UDefaultCameraShakeBase>(Effect), false);
	}

	// Cleanup dummy component via super
	Super::DestroyEffect();
	
	return true;
}

void UBSLEffect_CameraShake::GetCameraShakeSettings(const FBSLContextEffectData& ContextEffectData,
	FBSLCameraShakeSettings& OutCameraShakeSettings)
{
	for (const auto& data : ContextEffectData.CustomData)
	{
		if (const FBSLCameraShakeSettings* cameraShakeSettings = data.GetPtr<FBSLCameraShakeSettings>())
		{
			OutCameraShakeSettings = *cameraShakeSettings;
#if WITH_EDITORONLY_DATA
			WriteDebugMessage(ContextEffectData, FString::Printf(TEXT("%s: Set CameraShakeSettings for Effect %s on %s"), *GetClientServerContextString(),
				*ContextEffectData.EffectTag.ToString(), *ContextEffectData.StaticMeshComponent->GetOwner()->GetName()));
#endif
		}
	}
}
