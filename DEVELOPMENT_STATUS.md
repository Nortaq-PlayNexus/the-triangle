# DEVELOPMENT STATUS

_Last updated: 2026-09-14_

## Snapshot

- **Project:** THE TRIANGLE (`C:\Users\natha\projects\the-triangle`)
- **Engine:** Unreal Engine **5.8.2** at `D:\Program Files\UE_5.8`. Epic Games Launcher installed and logged in.
- **Toolchain:** Visual Studio Build Tools 2022 (17.14, MSVC 14.44.35207) + Windows SDK 10.0.26100.0 installed via VS Installer GUI. .NET Framework 4.8 SDK added for UBT SwarmInterface. CMake not required (UnrealBuildTool).
- **Machine:** i7-8700 / 24 GB / GTX 1080 8 GB / Windows 10 Home 10.0.19045 / ~1.7 TB free on C:.
- **Hardware targets:** 1080p, 60 FPS where achievable on the GTX 1080. SM5 + dynamic lighting, baked/static lighting where it wins. (See `PERFORMANCE.md`.)
- **Build status:** TriangleEditor build **succeeds** on UE 5.8.2 after fixing 8 API breaks (Enum.h→Class.h, EnhancedInput Context removal, AudioComponent AdjustVolume, USoundWaveProcedural methods, auto* range-for over TObjectPtr, GameMode DefaultPlayerStart, light casts/includes, SpawnActor FRotator overload). PIE runtime test pending.

## Phase status (from the design doc)

| Phase | Scope | Status |
|---|---|---|
| 1 | Project, first-person controller, interaction, save, Dream Director, Lucidity, first bedroom, playable | **Complete** — compiled, awaiting PIE runtime test |
| 2 | Underground station, train sequence, Triangle manifestation | **In progress** — station builder in development |
| 3 | Mansion, dream bleeding | Not started |
| 4 | Castle, Floating Man | Not started |
| 5 | Witches | Not started |
| 6 | Clock room | Not started |
| 7 | False awakenings | Not started |
| 8 | Notebook + memory system | Partially wired (notebook prop in slice) |
| 9 | Final fused dream | Not started |
| 10 | Ending + menu transformation | Not started |
| 11 | Optimization | Not started |
| 12 | QA | Not started |

## What is implemented (compiled, PIE test pending)

Everything below is complete, compiled C++ source in `Source/Triangle/…`. Code is verified to build on UE 5.8.2. Runtime behaviour is unverified until PIE.

- `Core/FTriangleGameplayStatics` — typed accessors for Dream Director, controller, HUD, save subsystem, lucidity, recognition.
- `Core/ATriangleGameMode` — spawns the Dream Director per session.
- `Core/UTriangleMotifSubsystem` — dream clock + recurring-motif scheduler (ClockStops12, Platform12Train) with cooldowns + recognition gates.
- `Save/UTriangleSaveGame` + `Save/UTriangleSaveSubsystem` — synchronous local save (player transform, lucidity, recognition, notebook/memory arrays, environment flags) + hotkey F5 save / F9 load.
- `Lucidity/ULTriangleLucidityComponent` — 0..100 hidden meter, Low/Medium/High/Max levels, gain via dream events, max event.
- `Lucidity/ULTriangleRecognitionComponent` — 0..100 hidden Recognition meter, five stages (Unknowing→Master), pattern-catalogue of witnessed manifestations, "it's here" sensing.
- `Interaction/UTriangleInteractableComponent` — reusable focus/interact component.
- `Characters/ATriangleCharacter` — Enhanced-Input first-person pawn (move/look/jump/interact/wake); sphere-overlap interaction finder; camera getter for the director; F5/F9/Q binds.
- `Characters/ATrianglePlayerController`, `UI/ATriangleHUD`, `UI/UTriangleHUDWidget` — in-C++ UMG HUD (interaction prompt, lucidity bar, optional recognition bar, subtitles), no content assets.
- `Dream/ATriangleDreamDirector` — presence model (0..1 ramp + hum + events), weighted manifestation pool (6 bodies), Master-sense hum-only appearances, motifs, wake withdrawal, corner-watch loop.
- `Dream/ATriangleTrianglePoint` — tiny emissive ceiling-corner point, manifest/hide, never inspectable; body #1 in the pool.
- `Dream/UTriangleManifestationDefinition` + `Dream/ETriangleManifestationKind` — data asset describing a Triangle body (kind, hum, duration, recognition gain, cooldown).
- `Dream/UTriangleFalseAwakeningManager` — layered false-wake cycles (Q), recognition-gated, difference lines, wake-reset signal to the Director.
- `Nightmare/UTriangleAnomalySubsystem` + `Nightmare/ATriangleAnomalyActor` — "cuts into the dream": razor gaps, corner splits, distant black points, impossible doorways; self-fading, budgeted, despawn-on-reset.
- `Audio/UTriangleAudioSubsystem` — persistent dream bed, named state tiers with fades, near-silence for manifestations, spatial one-shots, hum control.
- `Audio/ATriangleProceduralHumSource` — the Triangle's signature hum, generated entirely at runtime (two detuned partials, breathing) — zero audio assets required.
- `World/ATriangleRoomBuilder` — procedural childhood bedroom (shell, window, moon light, fog, bed, desk, player start at +5000 elevation) from engine BasicShapes.
- `World/ATriangleDoor` — refuses to open until High lucidity, then swings into darkness.
- `World/ATriangleNotebook` — three-page dream memory prop; adds lucidity + subtitles.
- `World/ATriangleStationBuilder` — procedural underground station (in development, Phase 2).
- `AI/ENightmareEntityState` — shared entity states (Idle/Follow/Observe/…/LeadPlayer/Wait) for later nightmare entities.

## Design-spec follow-up integrated (2026-09-14, pre-compile)

The additions the writer appended to the design prompt are implemented as real systems, not prose:

- Triangle as an **intrusion** → manifestation pool + anomaly cuts instead of a single monster body.
- The **Hum** → runtime-synthesised procedural drone driven by presence.
- **No single body** → six bodies, including zero-visible (hum-only).
- **"Cutting into the dream"** → razor-thin gaps, corner splits, distant points, doorway cuts.
- **Recognition system** → separate hidden stat with 5 stages; pre-visual "it's here" sensing.
- **Clock 12 / Platform 12** → recurring motifs on a dream clock.
- **Lucidity ≠ invincibility** → presence persists regardless; lucidity only grants pattern-rule breaks.
- **Unreliable wake** → false-awakening manager with layered differences.
- **Atmospheric discipline** → hum/corner simplicity; subtitles are sparse, anticipatory, never jumpscare.

## Technical decisions (see TECHNICAL_ARCHITECTURE.md for detail)

1. **Single runtime module `Triangle`** with internal folders matching the design doc structure — keeps build times and coupling manageable; split into plugins only if a system is reused across titles.
2. **Gameplay classes default to no-tick:** interaction scans on a timer, lucidity is event-driven, the director only ticks while the player exists.
3. **Enhanced Input configured entirely in C++** (actions + mapping context built at runtime) so the slice runs with zero content assets.
4. **Procedural environment using `/Engine/BasicShapes`** until licensed assets land; isolated in one builder class so it can be replaced atomically.
5. **C++ HUD widget** (no Blueprint dependency) for the same reason.
6. **Room elevation offset (+5000):** all procedural actors spawn at world altitude 5000 to avoid template terrain; the room interior is below this elevation.

## Blocked on this machine

- PIE runtime test — needs editor launch and play test.
- Then: station (Phase 2).

## Next actions

1. Launch editor, open `Triangle.uproject`, generate project files, compile.
2. PIE test: movement, interaction, save/load, lucidity loop, recognition, hum, wake.
3. Fix any runtime issues found.
4. Continue Phase 2 (underground station).