// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLContextEffectsLibrary.h"
#include "Subsystems/WorldSubsystem.h"
#include "BSLContextEffectsSubsystem.generated.h"

enum EPhysicalSurface : int;

/**
 *
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "BSLContextEffects"))
class BSLCONTEXTEFFECTS_API UBSLContextEffectsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UPROPERTY(config, EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, FGameplayTag> SurfaceTypeToContextMap;
};

/**
 * TSet can't be used as value in map so use this container
 */
UCLASS()
class BSLCONTEXTEFFECTS_API UBSLContextEffectsLibrarySet : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TSet<UBSLContextEffectsLibrary*> BSLContextEffectsLibraries;
};

/**
 * 
 */
UCLASS()
class BSLCONTEXTEFFECTS_API UBSLContextEffectsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	void SpawnContextEffects(const AActor* SpawningActor, FBSLContextEffectData ContextEffectData, TArray<UBSLLoadedEffect*>& OutEffects);
	
	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	void LoadAndAddContextEffectsLibraries(AActor* OwningActor, TSet<TSoftObjectPtr<UBSLContextEffectsLibrary>> ContextEffectsLibraries);

	// Called when library is done loading async
	void OnContextEffectsLibraryLoaded(TSoftObjectPtr<UBSLContextEffectsLibrary> LoadedContextEffectsLibrary, AActor* OwningActor);
	
	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	void UnloadAndRemoveContextEffectsLibraries(AActor* OwningActor);

private:

	UPROPERTY(Transient)
	TMap<AActor*, UBSLContextEffectsLibrarySet*> ActiveActorEffectsMap;
	
};
