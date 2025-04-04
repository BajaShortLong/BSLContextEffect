# BSL Context Effects
This plugin provides a framework for spawning effects such as particles, audio, camera shake, force feedback, and more. It's largely inspired by the Context Effects functionality in the Lyra project but includes improvements and provides an extensible framework to support any kind of effect or customization.

One of the best examples of contextual effects are footsteps. For a given surface type which a character steps on, you want to have different sounds, particles, and footprint decals. But you may want to change or modify these based on the character's anatomy or its current equipment. And going further, you may want to change or modify these based on the speed of the character or other gameplay states. It can quickly snowball into a long chain of conditions which becomes brittle and hard to modify.

In an attempt to solve this issue, this plugin combines predefined sets of effects and contexts along with capabilities to inject context and modify effects at runtime to respond to different scenarios and gameplay states. This allows us to emit a single event, such as `Event.FootStep.Walk`, and have the correct effects spawned after all our contexts are considered.

## Supported Effects
- NiagaraSystem
- SoundBase (MetaSounds)
- Force Feedback
- Camera Shakes (DefaultCameraShakeBase)

Though this plugin has built in effect types, you can implement any kind of effect you'd like by creating a subclass of UBSLLoadedEffect and implementing `SpawnEffect()` and `DestroyEffect()` as well as a subclass of UBSLEffectDefinition and implementing `MakeLoadedEffect()`. See the BSLEffects directory implementations of the existing effects. Its possible to implement effect types in Blueprint but probably easier in native.

## Multiplayer Support
This plugin fully supports single and multiplayer netmodes. However, it doesn't do any replication itself, rather it relys on things likely already replicated in most multiplayer project. For example, it makes use of AnimNotify and AnimNotifyStates on animations that are "replicated" via an AnimationBlueprint + movement component or RPC montages. It also makes use of GameplayTag state changes when hooked up to OnTagChange delegates common on ability systems (GAS, GMAS, etc).

Its worth noting that effects are only relevant to clients and should only be considered cosmetic.

## Architecture
[architecture diagram](/Resources/arch-diagram.png)
