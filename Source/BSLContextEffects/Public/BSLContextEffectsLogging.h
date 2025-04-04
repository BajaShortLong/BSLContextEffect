#pragma once

#include "BSLContextEffectsLogging.h"
#include "BSLEffect/BSLEffectData.h"
#include "Logging/LogMacros.h"

BSLCONTEXTEFFECTS_API DECLARE_LOG_CATEGORY_EXTERN(LogBSLContextEffects, Log, All);

static FString GetClientServerContextString(UObject* ContextObject = nullptr)
{
	ENetRole Role = ROLE_None;

	if (AActor* Actor = Cast<AActor>(ContextObject))
	{
		Role = Actor->GetLocalRole();
	}
	else if (UActorComponent* Component = Cast<UActorComponent>(ContextObject))
	{
		Role = Component->GetOwnerRole();
	}

	if (Role != ROLE_None)
	{
		return (Role == ROLE_Authority) ? TEXT("Server") : TEXT("Client");
	}
	else
	{
#if WITH_EDITOR
		if (GIsEditor)
		{
			extern ENGINE_API FString GPlayInEditorContextString;
			return GPlayInEditorContextString;
		}
#endif
	}

	return TEXT("[]");
}

#if WITH_EDITOR && WITH_EDITORONLY_DATA
BSLCONTEXTEFFECTS_API void WriteDebugMessage(const FBSLContextEffectData& EffectData, const FString& Message);
#endif

