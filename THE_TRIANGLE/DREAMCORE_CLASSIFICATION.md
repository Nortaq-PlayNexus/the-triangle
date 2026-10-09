# THE TRIANGLE — Dreamcore Classification Audit

**Audit date:** 2026-09-24  
**Project:** `C:\Users\natha\projects\the-triangle`  
**Overall status:** **Provisional Dreamcore Horror candidate; not a confirmed public classification.**

## Executive result

THE TRIANGLE strongly matches the **thematic and mechanical** parts of the proposed definition:

- it is an interactive first-person psychological horror game;
- its premise is interconnected nightmares and unreliable reality;
- it implements lucidity, recognition, false awakenings, motifs, impossible spaces, a recurring geometric presence, and a procedural hum;
- its horror style explicitly emphasizes dread, anticipation, silence, repetition, distorted familiarity, and recognition rather than combat or jump scares.

The personal-provenance requirement is supported by the user's project brief, which states that the game is based substantially on their own night-terror/nightmare experiences. That statement is preserved as **user-reported provenance**. The repository currently contains no dated dream journal, public interview, or design-source mapping that independently shows how specific personal experiences became the implemented world, events, entities, or mechanics.

Accordingly:

- **Base Dreamcore:** provisional candidate.
- **Dreamcore Horror:** provisional candidate.
- **Public confidence:** medium, pending creator evidence and a functioning current build.
- **Gameplay changes made to fit the label:** none.

## What is implemented

### Architecture

- Unreal Engine 5.8.2 target and a single C++ runtime module: `TECHNICAL_ARCHITECTURE.md:5-16`, `Triangle.uproject:6-21`.
- Native systems for game flow, player interaction, Lucidity, Recognition, Dream Director, anomalies, audio, save/load, HUD, and world builders.
- Procedural environment ownership in `Source/Triangle/Core/UTriangleGameFlowSubsystem.*`.
- Procedural BasicShapes construction in `Source/Triangle/World/ATriangleRoomBuilder.*` and `ATriangleStationBuilder.*`.

### Environments

- Childhood bedroom: implemented procedurally with bed, desk, notebook, stopped-at-twelve clock, moonlit window, fog, door, and an upper-corner Triangle point (`LEVEL_DESIGN.md:9-13`).
- Underground station / Platform 12: implemented procedurally with platforms, tracks, fluorescent strips, ventilation, benches, safety lights, Platform 12 sign, wrong train, and station fog (`LEVEL_DESIGN.md:15-17`).
- Clock room, mansion, castle/Floating Man, witches, transformed false-awakening bedroom, and final fused dream remain planned (`LEVEL_DESIGN.md:19-41`).

### Mechanics and horror systems

- Core loop: explore, observe, remember, recognize, question reality, become lucid, manipulate the dream, encounter recurring entities, discover connections, and lose certainty (`GAME_DESIGN.md:3-7`).
- No conventional combat; survival is observation, exploration, puzzles, and awareness (`GAME_DESIGN.md:5-7`).
- Lucidity is a 0–100 event-gated dream-awareness system (`Source/Triangle/Lucidity/ULTriangleLucidityComponent.h:19-31`).
- Recognition is a separate 0–100 system for learning to sense the Triangle before seeing it (`Source/Triangle/Lucidity/ULTriangleRecognitionComponent.h:21-41`).
- The Dream Director manages presence, six manifestation bodies, cooldowns, motifs, recognition, and anomalies (`DREAM_SYSTEM.md:9-21`; `Source/Triangle/Dream/ATriangleDreamDirector.*`).
- The hum is a real procedural audio source with a presence-driven state system (`AUDIO_DESIGN.md:20-35`; `Source/Triangle/Audio/ATriangleProceduralHumSource.*`).
- False awakenings are implemented as recognition-gated wake attempts with delayed wrong details and presence reset (`Source/Triangle/Dream/UTriangleFalseAwakeningManager.*`).
- The current source has four native core automation tests, but not an end-to-end slice acceptance test (`Source/Triangle/Tests/TriangleCoreTests.cpp`).

### Narrative

`STORY.md:3-26` describes a sequence of initially unrelated nightmares that are revealed as one dream, with a deliberately unexplained Triangle, a Floating Man who guides rather than chases, witches, false awakenings, a fused final dream, and an unresolved “DREAM AGAIN” structure. This is a strong thematic match, but most of the arc is planned rather than implemented.

## What is not yet evidenced

- No explicit “Dreamcore” definition or provenance map existed in the project before this documentation package.
- No dated source journal, private evidence record, or public interview maps personal experiences to individual game elements.
- No authored `.uasset`, `.umap`, mesh, texture, audio, or UI assets were found; the `Content` directories are empty.
- `Saved/AutoScreenshot.png` is generated/ignored and is not acceptance evidence.
- The current UE 5.8.2 installation is missing engine rule files, so a current build/PIE acceptance run cannot be claimed (`DEVELOPMENT_STATUS.md:7-12`, `BUILD_TROUBLESHOOTING.md:3-12`).
- False awakenings currently alter text, wake depth, presence, and related state; full environmental transformation remains a production phase (`DEVELOPMENT_STATUS.md:22-24`).

## Evidence matrix

| Proposed criterion | Local evidence | Assessment |
|---|---|---|
| Interactive work | C++ player, HUD, world builders, interaction, save/load | **Supported** |
| Personal dream/nightmare source | User brief states own night-terror basis | **User-reported; not independently mapped** |
| Material influence | Recurring nightmares, triangle presence, false awakening, wrongness, hum, environments | **Strong thematic/mechanical support; causal mapping still needed** |
| Traceable provenance | Local docs + user statement; no dated source artifact | **Incomplete** |
| Horror intention | README, project description, game design | **Supported** |
| Horror branch source | User statement + nightmare premise | **Probable, not independently corroborated** |
| Current finished experience | Pre-alpha/blockout; build gate | **Not supported** |

## Recommended classification language

### Internal

> Provisional Dreamcore Horror candidate based on the creator's reported personal night-terror provenance and the project's implemented dream-horror systems.

### Public, pending confirmation

> THE TRIANGLE is a first-person psychological dream-horror prototype whose design uses lucid awareness, recognition, false awakenings, recurring presences, and distorted familiar spaces. Its creator reports that the project is substantially informed by personal night-terror experiences; a public provenance record is still being developed.

### Do not say

- “THE TRIANGLE is the first Dreamcore Horror game.”
- “THE TRIANGLE proves the genre is new.”
- “The game is already a finished Dreamcore Horror title.”
- “The screenshots prove the intended atmosphere” when the available image is generated blockout output.

## Evidence-preserving next steps

1. Repair/verify the UE installation and run a real slice acceptance test.
2. Capture consented screenshots/trailer only after they represent the intended experience.
3. Create a private provenance map: source experience → design decision → implemented system → public-safe description.
4. Decide which details can be abstracted or kept private.
5. Obtain a creator-approved public statement if a directory entry is desired.
6. Revisit classification after the first public build/reveal; do not change gameplay solely to improve the label.
