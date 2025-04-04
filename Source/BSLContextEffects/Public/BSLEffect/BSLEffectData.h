#pragma once

#include "BSLGameplayTags.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponentPoolMethodEnum.h"
#include "StructUtils/InstancedStruct.h"

#include "BSLEffectData.generated.h"

class UBSLEffectModifierBase;
enum class ENCPoolMethod : uint8;
/**
 *
 */
UENUM()
enum EEffectsContextMatchType: int
{
	FirstMatch,
	ExactMatch
};

USTRUCT(BlueprintType)
struct FBSLContextEffectData
{
	GENERATED_BODY()
	~FBSLContextEffectData() = default;

	FBSLContextEffectData()
	{
		EffectTag = FGameplayTag::EmptyTag;
		Contexts = FGameplayTagContainer(BSLGameplayTags::ContextEffect_Context_Default);
		EffectModifiers = TArray<TObjectPtr<UBSLEffectModifierBase>>();
		ContextMatchType = FirstMatch;
		CustomData = TArray<FInstancedStruct>{};
		LoadedEffectTag = FName("None");
		bAttached = false;
		Socket = FName("None");
		bUseBoneRotation = false;
		StaticMeshComponent = nullptr;
		LocationOffset = FVector::ZeroVector;
		RotationOffset = FRotator::ZeroRotator;
		ComponentScale = FVector(1);
	}
	
	FBSLContextEffectData(FGameplayTag InEffectTag, FGameplayTagContainer InContexts, FGameplayTagContainer InEffectModifiers,
		EEffectsContextMatchType InContextMatchType, TArray<FInstancedStruct> InCustomData, FName InLoadedEffectTag, bool InAttached,
		FName InSocket, bool InUseBoneRotation, USceneComponent* InStaticMeshComponent, const FVector& InLocationOffset,
		const FRotator& InRotationOffset, const FVector& InComponentScale)
	{
		EffectTag = InEffectTag;
		Contexts = InContexts;
		EffectModifiers = TArray<TObjectPtr<UBSLEffectModifierBase>>();
		ContextMatchType = InContextMatchType;
		CustomData = InCustomData;
		LoadedEffectTag = InLoadedEffectTag;
		bAttached = InAttached;
		Socket = InSocket;
		bUseBoneRotation = InUseBoneRotation;
		StaticMeshComponent = InStaticMeshComponent;
		LocationOffset = InLocationOffset;
		RotationOffset = InRotationOffset;
		ComponentScale = InComponentScale;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag EffectTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Context")
	FGameplayTagContainer Contexts = FGameplayTagContainer(BSLGameplayTags::ContextEffect_Context_Default);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Context")
	TEnumAsByte<EEffectsContextMatchType> ContextMatchType = FirstMatch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TArray<FInstancedStruct> CustomData = TArray<FInstancedStruct>{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(ShowOnlyInnerProperties, FullyExpand=true), Category = "Effect")
	TArray<TObjectPtr<UBSLEffectModifierBase>> EffectModifiers;
	
	// Whether this effect should be attached to the Bone.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	bool bAttached = false;

	// Used for location of attachment, initial spawn location, and the start location of the trace 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	FName Socket = FName("None");
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Attachment")
	USceneComponent* StaticMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	FVector LocationOffset = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	FVector ComponentScale = FVector(1);

	// If true, uses the Bone rotation for initial rotation, otherwise a ZeroRotator
	// Its common to want an effect placed at the location of a socket/bone but not use the same rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	bool bUseBoneRotation = false;

	// Used to access an existing LoadedEffect, usually for lifecycle management. Generally set by the emitter if
	// lifecycle should be managed so setting this manually in the definition isn't useful.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Effect")
	FName LoadedEffectTag = FName("None");
};


/*
 * These are wrapper structs for customization of effect types or data required for effect modifiers.
 */

USTRUCT(BlueprintType)
struct FBSLSoundSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VolumeMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PitchMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	float StartTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Attachment")
	TEnumAsByte<EAttachLocation::Type> AttachLocation = EAttachLocation::KeepRelativeOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Attachment")
	bool bStopWhenAttachedToDestroyed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	USoundAttenuation* AttenuationSettings = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	USoundConcurrency* ConcurrencySettings = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	bool bAutoDestroy = true;
};

USTRUCT(BlueprintType)
struct FBSLNiagaraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	TEnumAsByte<EAttachLocation::Type> AttachLocation = EAttachLocation::KeepRelativeOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoDestroy = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoActivate = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENCPoolMethod PoolingMethod = ENCPoolMethod::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPreCullCheck = true;
	
};

USTRUCT(BlueprintType)
struct FBSLCameraShakeSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Scale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	FRotator UserPlaySpaceRot = FRotator::ZeroRotator;
};

USTRUCT(BlueprintType)
struct FBSLForceFeedbackSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UForceFeedbackEffect* ForceFeedbackEffect = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	USceneComponent* AttachToComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	FName AttachPointName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	FVector Location = FVector(ForceInit);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	FRotator Rotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Attachment")
	TEnumAsByte<EAttachLocation::Type> AttachLocation = EAttachLocation::KeepRelativeOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Attachment")
	bool bStopWhenAttachedToDestroyed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLooping = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IntensityMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	float StartTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	class UForceFeedbackAttenuation* AttenuationSettings = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoDestroy = true;
};

/*
 * Example of a wrapper for PostProcess effects. The PostProcessSettings struct is large so we probably only want to include
 * things as needed.
 *
USTRUCT(BlueprintType)
struct FBSLPostProcessData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MotionBlur")
	float MotionBlurAmount = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MotionBlur")
	float MotionBlurMax = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MotionBlur")
	int MotionBlurFPS = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MotionBlur")
	float MotionBlurPerObjectSize = 0.0f;
	
};
*/
