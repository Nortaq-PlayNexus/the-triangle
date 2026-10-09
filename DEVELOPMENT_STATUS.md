# DEVELOPMENT STATUS

_Last updated: 2026-09-24_

## Snapshot

- **Project:** THE TRIANGLE (`C:\Users\natha\projects\the-triangle`)
- **Engine:** Unreal Engine **5.8.2** at `D:\Program Files\UE_5.8`.
- **Target hardware:** i7-8700 / 24 GB / GTX 1080 8 GB / Windows 10 Home; 1080p target with a stable 30+ FPS floor.
- **Source state:** The bedroom/station vertical-slice source pass is complete for this revamp. Native code covers the environment flow, persistence, director safeguards, procedural audio, responsive HUD/input, and core automation tests; runtime validation remains pending.
- **Build gate:** `scripts/Test-UnrealInstall.ps1` confirms the Launcher precompiled rules assemblies, then reports **4 missing TextureFormatOodle redistributables** (2.9.5, 2.9.14, 2.9.15, 2.9.16); the optional Live Coding helper warning is non-fatal. UnrealBuildTool therefore fails before compiling `Triangle`. Epic Games Launcher → UE 5.8 → Manage → **Verify** is required before a trustworthy editor/game build or PIE smoke test.

## Phase status

| Phase | Scope | Status |
|---|---|---|
| 1 | Project, first-person controller, interaction, save, Dream Director, Lucidity, first bedroom | **Source complete; runtime gate pending engine verification** |
| 2 | Underground station, train sequence, Platform 12 motif | **Source integrated; runtime gate pending engine verification** |
| 3 | Mansion, dream bleeding | Planned after slice acceptance |
| 4 | Castle, Floating Man | Planned after slice acceptance |
| 5 | Witches | Planned after slice acceptance |
| 6 | Clock room | Planned after slice acceptance; bedroom clock motif now has a physical anchor |
| 7 | False awakenings | Manager and persistence hooks implemented; environment transformations remain a production phase |
| 8 | Notebook + memory system | Slice notebook and versioned interaction persistence implemented |
| 9 | Final fused dream | Planned |
| 10 | Ending + menu transformation | Planned |
| 11 | Optimization | Slice budgets/configuration added; profile after build repair |
| 12 | QA | Automation tests added; PIE acceptance pending |

## Revamp systems now in source

- `UTriangleGameFlowSubsystem` swaps and tears down procedural environments, relocates the pawn to tagged starts, and binds the bedroom door to the station transition.
- `ATriangleRoomBuilder` and `ATriangleStationBuilder` own all runtime actors they spawn, including a physical twelve clock, Platform 12 text, lights, player starts, and Triangle points.
- `ATriangleDoor` opens at High Lucidity, remains interactable, and requests a deliberate second-interaction transition.
- `ATriangleTrain` has real BasicShapes geometry, safe door toggling, a dark interior aperture, and interaction.
- `ATriangleDreamDirector` uses recognition gates, selection weights, per-body cooldowns, witnessed checks, and serialised manifestation timers.
- `UTriangleFalseAwakeningManager` has bounded wake attempts, cancellable delayed callbacks, and restorable wake depth.
- `UTriangleSaveGame` / `UTriangleSaveSubsystem` use schema versioning with explicit legacy migration, environment-relative player transforms, catalogue persistence, transactional `Applied`/`Queued`/`Failed` results, environment/door/notebook fields, safe restoration, and explicit slot deletion.
- `ULTriangleLucidityComponent` and `ULTriangleRecognitionComponent` restore values without replaying progression milestones.
- `UTriangleAudioSubsystem` and `ATriangleProceduralHumSource` provide persistent state routing and a buffered runtime hum.
- `ATriangleHUDWidget` / input work is source-complete around responsive anchors, queued subtitles, meter synchronization, gamepad input, pause, and transition fade.
- `Source/Triangle/Tests/TriangleCoreTests.cpp` covers manifestation bounds, recognition catalogue restoration, lucidity clamping, and save schema creation.

## Current blockers

1. **UE installation integrity:** the precompiled UBT rule assemblies are present, but four TextureFormatOodle redistributables required while constructing the editor target are missing (2.9.5, 2.9.14, 2.9.15, 2.9.16). The absent Live Coding helper library is reported as a non-fatal warning. Do not patch the engine installation manually; use Launcher Verify/repair.
2. **Runtime validation:** no PIE, packaged, or automation run can be claimed until the engine is repaired and `TriangleEditor` builds.

## Next actions

1. In Epic Games Launcher, verify UE 5.8.2 and rerun `scripts/Test-UnrealInstall.ps1` until it reports complete precompiled rules and native payloads.
2. Build with `scripts/Build-Triangle.ps1 -Target Editor`.
3. Run `scripts/Test-Triangle.ps1`.
4. PIE smoke test: bedroom spawn → notebook page → corner manifestation → High Lucidity door → second interaction → Platform 12 → wrong train → save → restart → load.
5. Profile the GTX 1080 and only then promote the slice from pre-alpha to alpha.
