# BSL Context Effects
This plugin provides a framework for spawning effects such as particles, audio, camera shake, force feedback, and more. It's largely inspired by the Context Effects functionality in the Lyra project but includes improvements and provides an extensible framework to support any kind of effect or customization.

One of the best examples of contextual effects are footsteps. For a given surface type which a character steps on, you want to have different sounds, particles, and footprint decals. But you may want to change or modify these based on the character's anatomy or its current equipment. And going further, you may want to change or modify these based on the speed of the character or other gameplay states. It can quickly snowball into a long chain of conditions which becomes brittle and hard to modify.

In an attempt to solve this issue, this plugin combines predefined sets of effects and contexts along with capabilities to inject context and modify effects at runtime to respond to different scenarios and gameplay states. This allows us to emit a single event, such as `Event.FootStep.Walk`, and have the correct effects spawned after all our contexts are considered.
> [!Note]
> Tested with UE 5.5

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

## BSLEffect Parameters
These are predefined parameters you would want to pass to effects after they are spawned. NiagaraSystems and MetaSounds are probably the most relavent effect types for this. See the [implementation for Niagara](https://github.com/BajaShortLong/BSLContextEffects/blob/1ed9378085199f7c25ebae5c4d40d00560530fe3/Source/BSLContextEffects/Private/BSLEffect/BSLEffects/BSLEffect_NiagaraSystem.cpp#L135)

Only a few parameters types are currently supported but more can be easily added in [BSLEffectParameters](Source/BSLContextEffects/Public/BSLEffect/BSLEffectParameters.h)
![](/Resources/bsleffectparameter.png)

## Effect Modifiers
Effect Modifiers modify the ContextEffectData payload before the effect is spawned. This is what you would use to inject context that must be queried at effect spawn, such as surface type for a footstep. The `BSLEffectModifier_Context_SurfaceType` can assist in just that and is a good example for how you can perform logic and add additional data (such as a FHitResult) to the payload for the effect to process.

`BSLEffectModifier_RelativeToMeshComponent` would be used frequently for effects spawned via BSLAnimNotify and BSLAnimNotifyState to set the spawn location relative to the animated mesh.

## Effect Handlers
The BSLHandlerInterface is applied to the ContextEffectComponent but can also be applied to any UObject you want to potentially hand off the effect to. In the Lyra project, footstep effects are managed in a Blueprint actor to prevent the footstep decals from dissapearing if the character is killed. Effects can be routed to different handlers by adding them to the `EffectRoutingRules` datatable on the BSLContextEffectsComponent which is a FGameplayTag (EffectTag) -> UObject association.
