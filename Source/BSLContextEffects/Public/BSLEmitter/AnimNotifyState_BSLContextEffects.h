// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotify_BSLContextEffects.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_BSLContextEffects.generated.h"

/**
 * 
 */
UCLASS(const, hidecategories=Object, CollapseCategories, Config = Game, meta=(DisplayName="Context Effect Notify State",
	AutoExpandCategories="ContextEffect"))
class BSLCONTEXTEFFECTS_API UAnimNotifyState_BSLContextEffects : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UAnimNotifyState_BSLContextEffects();
	
	// Overridden from UAnimNotifyState to provide custom notify name.
	virtual FString GetNotifyName_Implementation() const override;
	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
	
	FName GetLoadedEffectTag()const { return FName(GetFName().ToString() + FString::Printf(TEXT("_%s"), *ContextEffectData.EffectTag.ToString())); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (FullyExpand="true", ShowOnlyInnerProperties))
	FBSLContextEffectData ContextEffectData;

/*#if WITH_EDITORONLY_DATA
	UPROPERTY(Config, EditAnywhere, Category = "PreviewProperties")
	uint32 bPreviewInEditor : 1;

	UPROPERTY(EditAnywhere, Category = "PreviewProperties", meta = (EditCondition = "bPreviewInEditor"))
	FBSLContextEffectAnimNotifyPreviewSettings PreviewProperties;
#endif*/
};
