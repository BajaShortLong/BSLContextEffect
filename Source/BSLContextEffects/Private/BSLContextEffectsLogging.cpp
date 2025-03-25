#include "BSLContextEffectsLogging.h"

#include "BSLHandler/BSLContextEffectsComponent.h"
#include "BSLHandler/BSLHandlerInterface.h"

DEFINE_LOG_CATEGORY(LogBSLContextEffects);

#if WITH_EDITOR && WITH_EDITORONLY_DATA
void WriteDebugMessage(const FBSLContextEffectData& EffectData, const FString& Message)
{
	if (EffectData.StaticMeshComponent)
	{
		AActor* owner = EffectData.StaticMeshComponent->GetOwner();
		if (UBSLContextEffectsComponent* component = UBSLContextEffectsComponent::FindContextEffectsComponent(owner))
		{
			FGameplayTagContainer debugTags = IBSLHandlerInterface::Execute_GetDebugTags(component);
			if (debugTags.HasTag(EffectData.EffectTag))
			{
				UE_LOG(LogBSLContextEffects, VeryVerbose, TEXT("%s"), *Message);
			}
		}
	}
}
#endif
