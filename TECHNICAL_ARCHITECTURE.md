# TECHNICAL ARCHITECTURE

_Describes what is actually implemented / decided as of 2026-09-24._

## Engine & toolchain

- **Engine:** Unreal Engine **5.8.2** Launcher build at `D:\Program Files\UE_5.8`. `scripts/Test-UnrealInstall.ps1` validates the precompiled UBT rules assemblies and native editor payloads before project builds; the current installation is missing several native payloads and cannot yet reach Triangle compilation.
- **Toolchain:** Visual Studio Build Tools 2022 (17.14, MSVC 14.44.35207) + Windows SDK 10.0.26100.0.
- **CMake:** not required. UnrealBuildTool drives C++ builds; CMake is only needed for source-engine builds, which we are not doing.
- **Project language:** C++20 through UE 5.8 BuildSettings V7. Blueprints remain optional for designer-facing composition on top of native systems.

## Module layout

Single runtime module, internal folders mapped 1:1 to the design doc structure:

```
Triangle (Runtime)
├── Core/        FTriangleGameplayStatics, ATriangleGameMode,
│                UTriangleGameFlowSubsystem, UTriangleMotifSubsystem
├── Characters/  ATriangleCharacter (pawn), ATrianglePlayerController
├── Interaction/ UTriangleInteractableComponent
├── Lucidity/    ULTriangleLucidityComponent, ULTriangleRecognitionComponent (+ level enums)
├── Dream/       ATriangleDreamDirector, ATriangleTrianglePoint,
│                UTriangleManifestationDefinition (data asset), ETriangleManifestationKind,
│                UTriangleFalseAwakeningManager
├── Nightmare/   UTriangleAnomalySubsystem, ATriangleAnomalyActor
├── Audio/       UTriangleAudioSubsystem + ATriangleProceduralHumSource (WorldSubsystem)
├── Save/        UTriangleSaveGame, UTriangleSaveSubsystem (GameInstanceSubsystem)
├── UI/          ATriangleHUD, UTriangleHUDWidget (C++ UMG)
├── AI/          ENightmareEntityState (shared enum)
├── World/       ATriangleRoomBuilder, ATriangleStationBuilder, ATriangleDoor,
│                ATriangleNotebook, ATriangleTrain
└── Tests/       native automation coverage for progression, saves, and definitions
```

Splitting into additional modules happens only if a system is genuinely shared/reused; premature modules cost build time.

## Data model

- **Save** is a versioned `UTriangleSaveGame` (player transform plus environment-start-relative transform, lucidity, recognition + catalogue, memories, notebook count, environment, wake depth, and world flags) serialized via `UGameplayStatics::SaveGameToSlot` to `Dream_Slot_0`. New objects use a zero schema sentinel until the save boundary stamps the current schema; legacy catalogue counts and absolute transforms are migrated explicitly. `UTriangleSaveSubsystem` applies state without replaying progression events and reports `Applied`, `Queued`, or `Failed` for cross-environment loads.
- **Manifestations** are data-driven: `UTriangleManifestationDefinition` (DataAsset) states the body, hum level, duration window, recognition gain, recognition gate, selection weight, and cooldown. The Director holds a pool of six defaults; authoring can replace the pool per-instance.
- **Environment flow** is runtime-owned by `UTriangleGameFlowSubsystem`; builders are disposable factories and expose tagged `DreamStart` points for deterministic transitions.
- **Dream events** are designed to be data-driven. The slice keeps triggers in code inside the Dream Director; a full `UTriangleDreamEvent` DataAsset pass lands when events exceed the current loops.

## Event paths

- **Interaction finder** (pawns): timer-driven sphere overlap → nearest enabled `UTriangleInteractableComponent` within view cone → focus events + HUD prompt → `OnInteracted`.
- **Lucidity**: component clamps 0..100 and broadcasts on change/level. Levels: Low(0-30) Medium(30-60) High(60-90) Maximum(90-100). Gains only via dream events.
- **Recognition**: component clamps 0..100 with the five stages (Unknowing/Noticing/Recognizing/Anticipating/Master) and a deduplicated pattern catalogue of witnessed manifestation ids. Gains only via witnessed manifestations, motifs and wakes.
- **Presence**: the Director ramps 0..1 toward "wants presence" (manifestation active or corner visible) and broadcasts start/increase/disappear at crossings. Short motif/reveal hums use a separate transient layer, while the main hum (`UTriangleAudioSubsystem::SetHumPresence`) tracks the presence ramp with an attack lag.
- **Manifestations**: Director picks a weighted, recognition-gated body from the pool → corner point `Manifest()` or `UTriangleAnomalySubsystem::SpawnAnomaly()` → serialised resolution grants recognition only when the body is in view. Anomalies self-fade and self-despawn.
- **Motifs**: `UTriangleMotifSubsystem` owns the dream clock and schedules registered motifs (ClockStops12, Platform12Train) with cooldowns and recognition gates; the Director maps `OnMotif` to subtitles + recognition + hum swells and opens the wrong train when the station motif fires.
- **Wake**: character action (Q) → `UTriangleFalseAwakeningManager::TryWake()` (refused below Anticipating) → cancellable delayed subtitle + `OnFalseAwakening` → `OnWakeResetRequested` → Director `WithdrawPresence()` (hum off, anomalies withdrawn, corner hidden). Wake depth is restorable from the save schema, and load cancels stale callbacks.
- **Environment transition**: door interaction → `UTriangleGameFlowSubsystem::TransitionTo(UndergroundStation)` → old builder EndPlay destroys tracked actors → new builder creates tagged start → pawn teleport → Director clears environment-bound references. Transition failure is a first-class event, and the flow subsystem owns a balanced input lock for the full fade interval.
- **Dream Director**: acquires player + subsystems and can re-acquire after environment changes, then runs corner-watch/presence/sense logic. No per-frame work for static gameplay actors.

## Performance rules already applied

- Actor ticks: `bCanEverTick=false` wherever not needed (doors tick only while swinging; director ticks only while a player exists).
- Interaction scan at 0.08 s interval; lucidity is event-driven (no accumulation ticks).
- Procedural geometry reuses engine BasicShapes; builders explicitly clean up runtime actors during transitions.
- Fog, soft lights, and a bounded station light budget keep the GTX 1080 path viable.

## Integration points for future phases

- `ATriangleDreamDirector::TriggerTriangleManifest` and `ExecuteManifestation` are the entrances for Triangle presence — phases 2+ extend them with train/manifest/clock events and the HumanForm body (reserved in `ETriangleManifestationKind`).
- Dream bleeding will be implemented as director-driven actor/light/environment swaps, not new static content, so it can run on the GTX 1080.
- The false-awakening manager already layers wakes; later chapters drop in real second-bedroom levels keyed off `UTriangleFalseAwakeningManager::GetCurrentLayer()`.
- Save grows by adding members to `UTriangleSaveGame` + mappings in `UTriangleSaveSubsystem::BuildSnapshot`.