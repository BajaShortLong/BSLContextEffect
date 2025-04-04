// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BSLEffect/BSLEffectData.h"
#include "AnimNotify_BSLContextEffects.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct BSLCONTEXTEFFECTS_API FBSLContextEffectAnimNotifyPreviewSettings
{
	GENERATED_BODY()

	// If true, will attempt to match selected Surface Type to Context Tag via Project Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Preview)
	bool bPreviewPhysicalSurfaceAsContext = true;

	// Surface Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Preview, meta=(EditCondition="bPreviewPhysicalSurfaceAsContext"))
	TEnumAsByte<EPhysicalSurface> PreviewPhysicalSurface = EPhysicalSurface::SurfaceType_Default;

	// Preview Library
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Preview, meta = (AllowedClasses = "/Script/BSLContextEffects.BSLContextEffectsLibrary"))
	FSoftObjectPath PreviewContextEffectsLibrary;

	// Preview Context
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Preview)
	FGameplayTagContainer PreviewContexts;
};

/**
 * 
 */
UCLASS(const, hidecategories=Object, CollapseCategories, Config = Game, meta=(DisplayName="Context Effect Notify"))
class BSLCONTEXTEFFECTS_API UAnimNotify_BSLContextEffects : public UAnimNotify
{
	GENERATED_BODY()

public:
	
	UAnimNotify_BSLContextEffects();

	// Begin UAnimNotify interface
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
#if WITH_EDITOR
	virtual void ValidateAssociatedAssets() override;
#endif
	// End UAnimNotify interface

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable)
	void SetParameters(FBSLContextEffectData InContextEffectData, bool InPreviewInEditor,
	FBSLContextEffectAnimNotifyPreviewSettings InPreviewSettings);
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (FullyExpand="true", ShowOnlyInnerProperties))
	FBSLContextEffectData ContextEffectData;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Config, EditAnywhere, Category = "AnimNotify|PreviewProperties")
	uint32 bPreviewInEditor : 1;

	UPROPERTY(EditAnywhere, Category = "AnimNotify|PreviewProperties", meta = (EditCondition = "bPreviewInEditor"))
	FBSLContextEffectAnimNotifyPreviewSettings PreviewProperties;
#endif
};
