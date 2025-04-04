#include "BSLGameplayTags.h"

namespace BSLGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ContextEffect_Event, "ContextEffect.Event", "Abstract tag used to specify an event for an effect");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ContextEffect_Modifier, "ContextEffect.Modifier", "Abstract tag used to specify effect modifier classes for an event tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ContextEffect_Modifier_Context, "ContextEffect.Modifier.Context", "Abstract tag used to specify modifier classes that change the effect context");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ContextEffect_Context_Default, "ContextEffect.Context.Default", "Tag used to specify a default effect");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ContextEffect_Context_SurfaceType, "ContextEffect.Context.SurfaceType", "Physical Material SurfaceTypes");
}