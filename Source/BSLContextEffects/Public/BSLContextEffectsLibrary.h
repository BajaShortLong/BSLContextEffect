// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BSLEffect/BSLEffectData.h"
#include "BSLEffect/BSLEffectDefinition.h"
#include "UObject/Object.h"

#include "BSLContextEffectsLibrary.generated.h"

UENUM()
enum class EBSLContextEffectsLibraryLoadState : uint8 {
	Unloaded = 0,
	Loading = 1,
	Loaded = 2
};

/**
 *	Definition of a groups of effects tied to an effect tag and context
 */
USTRUCT(BlueprintType)
struct BSLCONTEXTEFFECTS_API FBSLContextEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EffectTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer Context = FGameplayTagContainer(BSLGameplayTags::ContextEffect_Context_Default);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta =(ShowOnlyInnerProperties, FullyExpand=true))
	TArray<TObjectPtr<UBSLEffectDefinition>> Effects;
};

/**
 *	A loaded and ready to use Context Effect
 */
USTRUCT(BlueprintType)
struct BSLCONTEXTEFFECTS_API FBSLActiveContextEffect
{
	GENERATED_BODY()
	~FBSLActiveContextEffect() = default;

	FBSLActiveContextEffect()
	{
		EffectTag = FGameplayTag::EmptyTag;
		Context = FGameplayTagContainer::EmptyContainer;
	}
	FBSLActiveContextEffect(FGameplayTag InEffectTag, FGameplayTagContainer InContext)
	{
		EffectTag = InEffectTag;
		Context = InContext;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EffectTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer Context = FGameplayTagContainer::EmptyContainer;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta =(ShowOnlyInnerProperties, FullyExpand=true))
	TArray<TObjectPtr<UBSLLoadedEffect>> Effects;
};

/*DECLARE_DYNAMIC_DELEGATE_TwoParams(FBSLContextEffectLibraryLoadingComplete,
	TSoftObjectPtr<UBSLContextEffectsLibrary>, LoadedContextEffectsLibrary, AActor*, OwningActor);*/

/**
 * 
 */
UCLASS(BlueprintType)
class BSLCONTEXTEFFECTS_API UBSLContextEffectsLibrary : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FBSLContextEffect> ContextEffects;

	/* This isn't used right now
	// Called when the library asset is loaded, effects will still need to be loaded
	FBSLContextEffectLibraryLoadingComplete LibraryLoadingComplete;*/

	UFUNCTION(BlueprintCallable)
	void GetEffects(const FBSLContextEffectData& ContextEffectData,	TArray<UBSLLoadedEffect*>& OutEffects);

	UFUNCTION(BlueprintCallable)
	void LoadEffects();

	EBSLContextEffectsLibraryLoadState GetContextEffectsLibraryLoadState() const;

	bool CheckEffectLoadingComplete() const;

protected:
	
	void LoadEffectsInternal();

	void ContextEffectLibraryLoadingComplete();
	void ContextEffectLoadingComplete(FBSLContextEffect LoadedEffectDefinition);

	// Context Effects currently loaded from all libraries
	UPROPERTY(Transient)
	TArray<FBSLActiveContextEffect> ActiveContextEffects;

	UPROPERTY(Transient)
	EBSLContextEffectsLibraryLoadState EffectsLoadState = EBSLContextEffectsLibraryLoadState::Unloaded;
	
};
