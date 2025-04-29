# BSL Context Effects
This plugin provides a framework for spawning effects such as particles, audio, camera shake, force feedback, and more. It's largely inspired by the Context Effects functionality in the Lyra project but includes improvements and provides an extensible framework to support any kind of effect or customization.

One of the best examples of contextual effects are footsteps. For a given surface type which a character steps on, you want to have different sounds, particles, and footprint decals. But you may want to change or modify these based on the character's anatomy or its current equipment. And going further, you may want to change or modify these based on the speed of the character or other gameplay states. It can quickly snowball into a long chain of conditions which becomes brittle and hard to modify.

In an attempt to solve this issue, this plugin combines predefined sets of effects and contexts along with capabilities to inject context and modify effects at runtime to respond to different scenarios and gameplay states. This allows us to emit a single event, such as `Event.FootStep.Walk`, and have the correct effects spawned after all our contexts are considered.

> [!Note]
> Tested with UE 5.5

> [!CAUTION]
> This is very much in active development and updates may make substantial changes

> [!WARNING]
> This plugin isn't suitable for all projects and likely overkill for any project that only has a single playable character and limited effect variation. You'd be better off using a system closer to Lyra's ContextEffects since this plugin would likely add unneccessary complexity.

> [!Tip]
> Check out the [sample project](https://github.com/BajaShortLong/BSLContextEffectsSampleProject)

## Supported Effects
- NiagaraSystem
- SoundBase (MetaSounds)
- Force Feedback
- Camera Shakes (DefaultCameraShakeBase)

Though this plugin has built in effect types, you can implement any kind of effect you'd like by creating a subclass of UBSLLoadedEffect and implementing `SpawnEffect()` and `DestroyEffect()` as well as a subclass of UBSLEffectDefinition and implementing `MakeLoadedEffect()`. See the [BSLEffects directory](/Source/BSLContextEffects/Public/BSLEffect/BSLEffects) implementations of the existing effects. Its possible to implement effect types in Blueprint but probably easier in native.

## Multiplayer Support
This plugin fully supports single and multiplayer netmodes. However, it doesn't do any replication itself, rather it relys on things likely already replicated in most multiplayer project. For example, it makes use of AnimNotify and AnimNotifyStates on animations that are "replicated" via an AnimationBlueprint + movement component or RPC montages. It also makes use of GameplayTag state changes when hooked up to OnTagChange delegates common on ability systems (GAS, GMAS, etc).
> [!Warning]
> Its worth noting that effects are only relevant to clients and should only be considered cosmetic.

## Architecture
![architecture diagram](/Resources/arch-diagram.png)

## Context
Persistent context, such as a character class or archetype, can be added to the ContextEffectsComponent at character spawn. For more transient context, injecting context via EffectModifiers is more approriate.

## Emitters
Though emitters don't have any explicit definition, any class that makes a call to the handler interface could be considered an emitter. The included emitters are AnimNotify_BSLContextEffects, AnimNotifyStates_BSLContextEffects, and the BSLStateChangeComponent. To call the handler interface, you should first setup a FBSLContextEffectData with at least an EffectTag and likely a valid StaticMeshComponent. Then make the call:
```
if (UBSLContextEffectsComponent* contextEffectComponent = UBSLContextEffectsComponent::FindContextEffectsComponent(owningActor))
{
    IBSLHandlerInterface::Execute_HandleStartEffect(contextEffectComponent, newContextEffectData);
}
```

## BSLEffect Parameters
These are predefined parameters you would want to pass to effects after they are spawned. NiagaraSystems and MetaSounds are probably the most relavent effect types for this. See the [implementation for Niagara](https://github.com/BajaShortLong/BSLContextEffects/blob/1ed9378085199f7c25ebae5c4d40d00560530fe3/Source/BSLContextEffects/Private/BSLEffect/BSLEffects/BSLEffect_NiagaraSystem.cpp#L135)

Only a few parameters types are currently supported but more can be easily added in [BSLEffectParameters](Source/BSLContextEffects/Public/BSLEffect/BSLEffectParameters.h)
![](/Resources/bsleffectparameter.png)

## Effect Modifiers
Effect Modifiers modify the FBSLContextEffectData payload before the effect is spawned. This is what you would use to inject context that must be queried at effect spawn, such as surface type for a footstep. The `BSLEffectModifier_Context_SurfaceType` can assist in just that and is a good example for how you can perform logic and add additional data (such as a FHitResult) to the payload for the effect to process.

`BSLEffectModifier_RelativeToMeshComponent` would be used frequently for effects spawned via BSLAnimNotify and BSLAnimNotifyState to set the spawn location relative to the animated mesh.

## Effect Handlers
The BSLHandlerInterface is applied to the ContextEffectComponent but can also be applied to any UObject you want to potentially hand off the effect to for additional processing. In the Lyra project, footstep particle effects are managed in a Blueprint actor to prevent the footstep decals from dissapearing when the character is killed. Effects can be routed to different handlers by adding them to the `EffectRoutingRules` datatable on the BSLContextEffectsComponent which is a FGameplayTag (EffectTag) -> UObject association.

## Context Effects Libraries
Libraries contain all effect definitions and are created in the content browser by right clicking and selecting `ContextEffects->BSLContextEffectsLibrary`. Libraries are then added to the BSLContextEffectComponent, either in default settings or at runtime. Libraries and the effect assets are added as SoftObjects and are loaded asynchronously so you'll want to manage when loading is initiated. Some thought should be put in to how libraries are organized and what effects are added to each one. For example, you might want different particles and sounds effects for different character skins so you may have a default effect library as well as a library for each character skin with modified context and effect parameters.

## State Change Component
Sometimes you just want to have effects that respond to changes in the character's state, without animations or specific actions. For example, you could play a looping air whoosh sound when `State.Falling` is added and destroy it when its removed. The BSLStateChangeComponent can orchestrate this with a little bit of boilerplate. Create a child of BSLStateChangeComponent and bind to your state change delegate. This is an example using GMAS but it could easily work with GAS or a custom ability system.
```
void UCustomStateChangeComponent::BindToSourceOfTruth_Implementation()
{
	if (ACustomCharacter* character = Cast<ACustomCharacter>(GetOwner()))
	{
		// Bind to ASC so we can watch for tag changes
		AbilitySystemComponent = UCustomGMASAbilitySystemComponent::FindAbilitySystemComponent(character);
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->OnActiveTagsChanged.AddUniqueDynamic(this, &ThisClass::ForwardActiveTagsChanged);
		}
	}
}

void UCustomStateChangeComponent::ForwardActiveTagsChanged(FGameplayTagContainer AddedTags,
	FGameplayTagContainer RemovedTags)
{
    /* Passing active tags is optional but allows for more flexibility with things like "stacking" tags. 
    *  You might have a GameplayEffect that can be applied multiple times and only want to remove the ContextEffect when there are no more of the
    *  GameplayEffects enabled. 
    */
	HandleActiveTagsChanged(AddedTags, RemovedTags, AbilitySystemComponent->GetActiveTags());
}
```
