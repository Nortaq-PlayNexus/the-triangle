# TECHNICAL ARCHITECTURE

_Describes what is actually implemented / decided as of 2026-09-14._

## Engine & toolchain

- **Engine:** Unreal Engine 5.x via Epic Games Launcher (`Installed` build). Not installed on this machine yet (Epic Games Launcher installed; engine download is the next gated step).
- **Toolchain:** Visual Studio Build Tools 2022 (17.14) + MSVC x64 toolset + Windows 11 SDK (10.0.22621) — workload install in progress.
- **CMake:** not required. UnrealBuildTool drives C++ builds; CMake is only needed for source-engine builds, which we are not doing.
- **Project language:** C++17 (UE default). Blueprints are allowed for designer-facing composition on top of C++ systems.

## Module layout

Single runtime module, internal folders mapped 1:1 to the design doc structure:

```
Triangle (Runtime)
├── Core/        FTriangleGameplayStatics, ATriangleGameMode, UTriangleMotifSubsystem
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
└── World/       ATriangleRoomBuilder, ATriangleDoor, ATriangleNotebook
```

Splitting into additional modules happens only if a system is genuinely shared/reused; premature modules cost build time.

## Data model

- **Save** is a `UTriangleSaveGame` (player transform, lucidity, recognition, memories, notebook pages, environment flags) serialized via `UGameplayStatics::SaveGameToSlot` to `Dream_Slot_0`.
- **Manifestations** are data-driven: `UTriangleManifestationDefinition` (DataAsset) states the body, hum level, duration window, recognition gain and cooldown. The Director holds a pool of six defaults; authoring can replace the pool per-instance.
- **Dream events** are designed to be data-driven. The slice keeps triggers in code inside the Dream Director; a full `UTriangleDreamEvent` DataAsset pass lands when events exceed the current loops.

## Event paths

- **Interaction finder** (pawns): timer-driven sphere overlap → nearest enabled `UTriangleInteractableComponent` within view cone → focus events + HUD prompt → `OnInteracted`.
- **Lucidity**: component clamps 0..100 and broadcasts on change/level. Levels: Low(0-30) Medium(30-60) High(60-90) Maximum(90-100). Gains only via dream events.
- **Recognition**: component clamps 0..100 with the five stages (Unknowing/Noticing/Recognizing/Anticipating/Master) and a deduplicated pattern catalogue of witnessed manifestation ids. Gains only via witnessed manifestations, motifs and wakes.
- **Presence**: the Director ramps 0..1 toward "wants presence" (manifestation active, corner visible, or Master sense active) and broadcasts start/increase/disappear at crossings. The hum (`UTriangleAudioSubsystem::SetHumPresence`) tracks it with an attack lag.
- **Manifestations**: Director picks a weighted body from the pool → corner point `Manifest()` or `UTriangleAnomalySubsystem::SpawnAnomaly()` → recognition granted when the appearance resolves. Anomalies self-fade and self-despawn.
- **Motifs**: `UTriangleMotifSubsystem` owns the dream clock and schedules registered motifs (ClockStops12, Platform12Train) with cooldowns and recognition gates; the Director maps `OnMotif` to subtitles + recognition + hum swells.
- **Wake**: character action (Q) → `UTriangleFalseAwakeningManager::TryWake()` (refused below Anticipating) → false wake subtitles + `OnFalseAwakening` → `OnWakeResetRequested` → Director `WithdrawPresence()` (hum off, anomalies withdrawn, corner hidden).
- **Dream Director**: acquires player + subsystems once, then per-tick corner-watch/presence/sense logic. No per-frame work for gameplay actors.

## Performance rules already applied

- Actor ticks: `bCanEverTick=false` wherever not needed (doors tick only while swinging; director ticks only while a player exists).
- Interaction scan at 0.08 s interval; lucidity is event-driven (no accumulation ticks).
- Procedural geometry reuses engine BasicShapes (few draw calls for a small room).
- Fog, soft lights, no real-time shadows beyond the directional moon light in the slice room.

## Integration points for future phases

- `ATriangleDreamDirector::TriggerTriangleManifest` and `ExecuteManifestation` are the entrances for Triangle presence — phases 2+ extend them with train/manifest/clock events and the HumanForm body (reserved in `ETriangleManifestationKind`).
- Dream bleeding will be implemented as director-driven actor/light/environment swaps, not new static content, so it can run on the GTX 1080.
- The false-awakening manager already layers wakes; later chapters drop in real second-bedroom levels keyed off `UTriangleFalseAwakeningManager::GetCurrentLayer()`.
- Save grows by adding members to `UTriangleSaveGame` + mappings in `UTriangleSaveSubsystem::BuildSnapshot`.