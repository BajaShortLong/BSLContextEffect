// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffectData.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "BSLEffectDefinition.generated.h"

class UBSLLoadedEffect;

// An unloaded specification for an effect
// Child EffectDefinitions can be defined in blueprint and must override GetAllowedAssetClass which specifies what type of
// effect it is. See examples in BSLEffect/BSLEffects
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, Abstract, AutoExpandCategories="UBSLEffectBase")
class BSLCONTEXTEFFECTS_API UBSLEffectDefinition : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (GetAllowedClasses="GetAllowedAssetClass", AllowAbstract="false", ExactAccess="false"))
	FSoftObjectPath EffectAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BaseStruct="/Script/BSLContextEffects.BSLEffectParameter", FullyExpand=true))
	TArray<FInstancedStruct> EffectParameters;

	// Whether this effect should only spawn on the locally controlled player's client
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLocallyControlledOnly = false;

	// Specifies the underlying asset type this Effect provides
	UFUNCTION(BlueprintNativeEvent)
	TArray<UClass*> GetAllowedAssetClass() const;

	// Must be overwritten by child class
	UFUNCTION(BlueprintNativeEvent)
	UBSLLoadedEffect* MakeLoadedEffect();

};

// A Context Effect that is loaded
UCLASS(BlueprintType, Blueprintable)
class BSLCONTEXTEFFECTS_API UBSLLoadedEffect : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObject> Effect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBSLEffectDefinition> EffectDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UActorComponent> EffectComponent;

	/** Array of tags mostly used for lifecycle tracking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Tags)
	TArray<FName> LoadedEffectTags;

	virtual bool SpawnEffect(const FBSLContextEffectData& ContextEffectData);
	virtual bool DestroyEffect();
	
	/** See if this component contains the supplied tag */
	UFUNCTION(BlueprintCallable)
	bool EffectHasTag(FName Tag) const;
};


