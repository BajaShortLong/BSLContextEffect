// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLHandlerInterface.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "BSLContextEffectsComponent.generated.h"

class UBSLEffectModifierBase;
class UBSLEffectModifierBase;
class UBSLContextEffectsLibrary;

// Describes relationship between GameplayTag -> BSLHandlerInterface UObject
// Primarily used to route effect data to actors for further logic and customization of effect
USTRUCT(BlueprintType)
struct FBSLContextEffectRoutingRule : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer MatchTags = FGameplayTagContainer();
		
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Handler",
		meta=(MustImplement="BSLHandlerInterface", DisallowedClasses="BSLContextEffectsComponent"))
	TSubclassOf<UObject> EffectHandlerClass = UObject::StaticClass();
};

// Describes relationship between GameplayTag -> BSLEffectModifier
USTRUCT(BlueprintType)
struct FBSLEffectModifierDefinition : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Tags")
	FGameplayTag MatchTag = FGameplayTag::EmptyTag;
		
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "EffectModifier", meta = (AllowedClasses = "/Script/BSLContextEffects.BSLEffectModifierBase"))
	TSubclassOf<UBSLEffectModifierBase> EffectModifierClass = UObject::StaticClass();
};

UCLASS(ClassGroup=(Custom), hidecategories = (Variable, Tags, ComponentTick, ComponentReplication, Activation, Cooking, AssetUserData, Collision),
	CollapseCategories, meta=(BlueprintSpawnableComponent))
class BSLCONTEXTEFFECTS_API UBSLContextEffectsComponent : public UActorComponent, public IBSLHandlerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBSLContextEffectsComponent();

	// Returns the AbilitySystemComponent component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "ContextEffects")
	static UBSLContextEffectsComponent* FindContextEffectsComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UBSLContextEffectsComponent>() : nullptr); }
	
	/* BSLHandlerInteface */
	UFUNCTION(BlueprintCallable)
	virtual void HandleStartEffect_Implementation(FBSLContextEffectData& EffectData) override;
	UFUNCTION(BlueprintCallable)
	virtual void HandleEndEffect_Implementation(FName LoadedEffectTag) override;
	UFUNCTION(BlueprintCallable)
	virtual FGameplayTagContainer GetDebugTags_Implementation() override;
	/* BSLHandlerInteface */

	UFUNCTION(BlueprintCallable)
	void UpdateEffectContexts(FGameplayTagContainer NewEffectContexts);

	UFUNCTION(BlueprintCallable)
	void UpdateLibraries(TSet<TSoftObjectPtr<UBSLContextEffectsLibrary>> NewContextEffectsLibraries);

	// Returns the effect of specified class and event tag if one exists for this ContextEffectComponent
	/*UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	UObject* GetContextEffect(TSubclassOf<UBSLLoadedEffect> EffectType, FGameplayTag EventTag);*/

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the game ends
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Register our GameplayTag -> BSLContextEffects Actor rules
	// Will route things like AnimEffect.FootStep.Run to the Footstep Actor for this component's Owner
	void RegisterRoutingRules();
	void RouteEffects(FBSLContextEffectData& EffectData);

	//void RegisterEffectModifiers();
	void ModifyEffect(FBSLContextEffectData& EffectData) const;

public:

	// Default Contexts
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer DefaultEffectContexts;

	// Default Libraries for this Actor
	UPROPERTY(EditAnywhere)
	TSet<TSoftObjectPtr<UBSLContextEffectsLibrary>> DefaultContextEffectsLibraries;

	// Datatable containing our EffectRoutingRules
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect Routing Rules")
	TObjectPtr<UDataTable> RoutingRulesTable;

	UPROPERTY(Transient)
	FGameplayTagContainer CurrentContexts;
	
#if WITH_EDITORONLY_DATA
	// AnimEffect tags to debug
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Debug")
	FGameplayTagContainer DebugTags;
#endif

	
protected:
	
	UPROPERTY(Transient)
	TSet<TSoftObjectPtr<UBSLContextEffectsLibrary>> CurrentContextEffectsLibraries;

	UPROPERTY(Transient)
	TArray<UBSLLoadedEffect*> ActiveContextEffects;
	
	TArray<FBSLContextEffectRoutingRule*> EffectRoutingRules;

	UPROPERTY(Transient)
	TMap<FGameplayTag, AActor*> RuleCurrentHandlerMap;
};
