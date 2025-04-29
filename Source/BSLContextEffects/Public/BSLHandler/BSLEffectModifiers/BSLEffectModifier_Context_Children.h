// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLEffectModifierBase.h"
#include "BSLEffectModifier_Context_Children.generated.h"

/**
 * Given a ParentTag, will search the ContextEffectComponent Context and inject any children tags found
 * into the ContextEffectData Context 
 */
UCLASS(DisplayName="Context Children Tags")
class BSLCONTEXTEFFECTS_API UBSLEffectModifier_Context_Children : public UBSLEffectModifierBase
{
	GENERATED_BODY()

public:
	
	virtual bool ModifyEffect_Implementation(FBSLContextEffectData& ContextEffectData) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ShowInnerProperties))
	FGameplayTag ParentTag;

	// Default tag to add if no children tags are found
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ShowInnerProperties))
	FGameplayTag DefaultTag = BSLGameplayTags::ContextEffect_Context_Default;
};
