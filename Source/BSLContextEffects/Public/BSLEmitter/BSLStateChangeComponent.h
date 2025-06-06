// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BSLEffect/BSLEffectData.h"
#include "BSLEffect/BSLEffectDefinition.h"
#include "Components/ActorComponent.h"
#include "BSLStateChangeComponent.generated.h"


class UBSLContextEffectsComponent;

// Rows that describe a translation rule between "State" tags and ContextEffects
USTRUCT(BlueprintType)
struct FBSLStateEffectRule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag StateTag;
	
	// Apply EffectData when this State is added
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bOnAdded = false;

	// Apply EffectData when this State is removed
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition="!bDestroyOnTagRemoved"))
	bool bOnRemoved = false;

	// Only valid when bOnAdded=true. This will override bOnRemoved.
	// Will send a request to destroy any spawned effects when the StateTag is removed.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Removal")
	bool bDestroyOnTagRemoved = false;

	// Wait until all instances of tag have been removed before killing context effects.
	// Requires active tags are passed to HandleActiveTagsChanged()
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "bOnAdded"), Category="Removal")
	bool bWaitForAllInstances = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "bOnAdded", EditConditionHides))
	FBSLContextEffectData AddedEffectData;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "bOnRemoved", EditConditionHides))
	FBSLContextEffectData RemovedEffectData;
};

UCLASS(BlueprintType)
class UBSLStateEffectRules : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty="{StateTag}"))
	TArray<FBSLStateEffectRule> Rules;
};

/*
 *	Component that watches for gameplay tag changes and fires off effects based on rules in StateEffectRulesAsset
 *	To implement, BindToSourceOfTruth() must be overridden to bind some GameplayTag event to HandleActiveTagsChanged
 *	Using GMAS and a child of UGMC_AbilitySystemComponent, this could look like:
 *	
 *		if (UGMC_AbilitySystemComponent* ASC = UCustomAbilitySystemComponent::FindAbilitySystemComponent(character))
 *		{
 *			ASC->OnActiveTagsChanged.AddUniqueDynamic(this, &ThisClass::HandleActiveTagsChanged);
 *		}
 */
UCLASS(Blueprintable, Abstract, ClassGroup=(Custom), hidecategories = (Variable, Tags, ComponentTick, ComponentReplication, Activation, Cooking, AssetUserData, Collision),
	CollapseCategories, meta=(BlueprintSpawnableComponent))
class BSLCONTEXTEFFECTS_API UBSLStateChangeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBSLStateChangeComponent();

	// This function must be overridden and setup to bind to the event which broadcasts GameplayTag changes
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="State")
	void BindToSourceOfTruth();

	// Override to bind the mesh component used in EffectData originating from this component
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="EffectData")
	void SetCacheables();

protected:
	
	virtual void BeginPlay() override;
	virtual void Initialize();
	
	void RegisterStateEffectRules();
	virtual void OnStateEffectRulesLoaded();

	// Support ContextEffects outside of animations by being able to react to `State.*` tag changes
	UFUNCTION(BlueprintCallable)
	virtual void HandleActiveTagsChanged(FGameplayTagContainer AddedTags, FGameplayTagContainer RemovedTags, const FGameplayTagContainer CurrentTags);

	virtual FBSLContextEffectData PrepareEffectData(FGameplayTag EffectTag);
	virtual void ProcessEffectTransform(FBSLContextEffectData& EffectData);
	
	virtual FName GetLoadedEffectTag(FGameplayTag Tag)const;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Rules")
	TSoftObjectPtr<UBSLStateEffectRules> StateEffectRulesAsset;

protected:

	// Map with "State." tag corresponding to index in StateEffectTranslations array
	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, int> StateAnimEffectMap;

	FGameplayTagContainer RegisteredTagsForAdded;
	FGameplayTagContainer RegisteredTagsForRemoved;
	
	UPROPERTY()
	TObjectPtr<UBSLStateEffectRules> StateEffectRules;

	TObjectPtr<USkeletalMeshComponent> MeshComponent;
	TObjectPtr<UBSLContextEffectsComponent> ContextEffectsComponent;
};
