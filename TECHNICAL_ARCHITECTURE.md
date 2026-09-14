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
├── Core/        FTriangleGameplayStatics, ATriangleGameMode
├── Characters/  ATriangleCharacter (pawn), ATrianglePlayerController
├── Interaction/ UTriangleInteractableComponent
├── Lucidity/    ULTriangleLucidityComponent (+ level enum)
├── Dream/       ATriangleDreamDirector, ATriangleTrianglePoint
├── Nightmare/   (reserved — entities)
├── Audio/       UTriangleAudioSubsystem (WorldSubsystem)
├── Save/        UTriangleSaveGame, UTriangleSaveSubsystem (GameInstanceSubsystem)
├── UI/          ATriangleHUD, UTriangleHUDWidget (C++ UMG)
├── AI/          ENightmareEntityState (shared enum)
└── World/       ATriangleRoomBuilder, ATriangleDoor, ATriangleNotebook
```

Splitting into additional modules happens only if a system is genuinely shared/reused; premature modules cost build time.

## Data model

- **Save** is a `UTriangleSaveGame` (player transform, lucidity, memories, notebook pages, environment flags) serialized via `UGameplayStatics::SaveGameToSlot` to `Dream_Slot_0`.
- **Dream events** are designed to be data-driven (`UTriangleDreamEvent` DataAsset with ID/Trigger/Condition/Environment/Audio/VisualEffect/Entity/Duration/NextEvent). The slice keeps triggers in code inside the Dream Director; the DataAsset pass is planned when events exceed the corner-watch loop.

## Event paths

- **Interaction finder** (pawns): timer-driven sphere overlap → nearest enabled `UTriangleInteractableComponent` within view cone → focus events + HUD prompt → `OnInteracted`.
- **Lucidity**: `ULTriangleLucidityComponent` clamps 0..100 and broadcasts on change/level. Levels: Low(0-30) Medium(30-60) High(60-90) Maximum(90-100). Gains only via dream events.
- **Dream Director**: acquires player + environment once, then per-tick corner-watch logic; drives `ATriangleTrianglePoint` manifestations and lucidity-level scripted beats. No per-frame work for gameplay actors.

## Performance rules already applied

- Actor ticks: `bCanEverTick=false` wherever not needed (doors tick only while swinging; director ticks only while a player exists).
- Interaction scan at 0.08 s interval; lucidity is event-driven (no accumulation ticks).
- Procedural geometry reuses engine BasicShapes (few draw calls for a small room).
- Fog, soft lights, no real-time shadows beyond the directional moon light in the slice room.

## Integration points for future phases

- `ATriangleDreamDirector::TriggerTriangleManifest` is the single entrance for the Triangle presence — phases 2+ extend it with train/manifest/clock events.
- Dream bleeding will be implemented as director-driven actor/light/environment swaps, not new static content, so it can run on the GTX 1080.
- Save grows by adding members to `UTriangleSaveGame` + mappings in `UTriangleSaveSubsystem::BuildSnapshot`.