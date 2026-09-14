# DEVELOPMENT STATUS

_Last updated: 2026-09-14_

## Snapshot

- **Project:** THE TRIANGLE (`C:\Users\natha\projects\the-triangle`)
- **Engine:** Unreal Engine 5.x — **not installed yet.** Epic Games Launcher installed; engine download is the next gated step (needs an Epic account login in the launcher GUI).
- **Toolchain:** Visual Studio Build Tools 2022 present; **MSVC C++ toolchain + Windows 10/11 SDK being installed** (workload install running). CMake is **not** required for the launcher-installed-engine route (UnrealBuildTool handles builds) and is therefore not installed.
- **Machine:** i7-8700 / 24 GB / GTX 1080 8 GB / Windows 10 Home 10.0.19045 / ~1.7 TB free on C:.
- **Hardware targets:** 1080p, 60 FPS where achievable on the GTX 1080. No hardware Lumen / Nanite at max settings; SM5 + dynamic lighting, baked/static lighting where it wins on performance. (See `PERFORMANCE.md`.)

## Phase status (from the design doc)

| Phase | Scope | Status |
|---|---|---|
| 1 | Project, first-person controller, interaction, save, Dream Director, Lucidity, first bedroom, playable | **In progress** — all C++ written; compile/run pending engine |
| 2 | Underground station, train sequence, Triangle manifestation | Not started |
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

## What is implemented (written, awaiting first compile)

Everything below is complete C++ source in `Source/Triangle/…`. **None of it has compiled or run yet** — code is unverified until the engine is installed and the first build passes.

- `Core/FTriangleGameplayStatics` — typed accessors for Dream Director, controller, HUD, save subsystem, lucidity, recognition.
- `Core/ATriangleGameMode` — spawns the Dream Director per session.
- `Core/UTriangleMotifSubsystem` — dream clock + recurring-motif scheduler (ClockStops12, Platform12Train) with cooldowns + recognition gates.
- `Save/UTriangleSaveGame` + `Save/UTriangleSaveSubsystem` — synchronous local save (player transform, lucidity, recognition, notebook/memory arrays, environment flags) + hotkey F5 save / F9 load.
- `Lucidity/ULTriangleLucidityComponent` — 0..100 hidden meter, Low/Medium/High/Max levels, gain via dream events, max event.
- `Lucidity/ULTriangleRecognitionComponent` — 0..100 hidden Recognition meter, five stages (Unknowing→Master), pattern-catalogue of witnessed manifestations, "it's here" sensing.
- `Interaction/UTriangleInteractableComponent` — reusable focus/interact component.
- `Characters/ATriangleCharacter` — Enhanced-Input first-person pawn (move/look/jump/interact/wake); sphere-overlap interaction finder; camera getter for the director; F5/F9/Q binds.
- `Characters/ATrianglePlayerController`, `UI/ATriangleHUD`, `UI/UTriangleHUDWidget` — in-C++ UMG HUD (interaction prompt, lucidity bar, optional recognition bar, subtitles), no content assets.
- `Dream/ATriangleDreamDirector` — presence model (0..1 ramp + hum + events), weighted manifestation pool, Master-sense hum-only appearances, motifs, wake withdrawal, corner-watch loop.
- `Dream/ATriangleTrianglePoint` — tiny emissive ceiling-corner point, manifest/hide, never inspectable; body #1 in the pool.
- `Dream/UTriangleManifestationDefinition` + `Dream/ETriangleManifestationKind` — data asset describing a Triangle body (kind, hum, duration, recognition gain, cooldown).
- `Dream/UTriangleFalseAwakeningManager` — layered false-wake cycles (Q), recognition-gated, difference lines, wake-reset signal to the Director.
- `Nightmare/UTriangleAnomalySubsystem` + `Nightmare/ATriangleAnomalyActor` — "cuts into the dream": razor gaps, corner splits, distant black points, impossible doorways; self-fading, budgeted, despawn-on-reset.
- `Audio/UTriangleAudioSubsystem` — persistent dream bed, named state tiers with fades, near-silence for manifestations, spatial one-shots, hum control.
- `Audio/ATriangleProceduralHumSource` — the Triangle's signature hum, generated entirely at runtime (two detuned partials, breathing) — zero audio assets required.
- `World/ATriangleRoomBuilder` — procedural childhood bedroom (shell, window, moon light, fog, bed, desk, player start) from engine BasicShapes.
- `World/ATriangleDoor` — refuses to open until High lucidity, then swings into darkness.
- `World/ATriangleNotebook` — three-page dream memory prop; adds lucidity + subtitles.
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

## Blocked on this machine

- UE engine download (+ Epic account login) — user action required in the Epic Games Launcher GUI.
- First compile + launch + movement/interaction test.
- Then: station (Phase 2).

## Next actions

1. Install engine: Epic Games Launcher → log in → Library → Unreal Engine 5.6.x → Install (use `Installed`).
2. When done, revalidate `Triangle.uproject` EngineAssociation value matches the installed version.
3. Generate project files + open the editor; compile module; fix/verrify; run PIE; test movement, interaction, save/load, lucidity loop.