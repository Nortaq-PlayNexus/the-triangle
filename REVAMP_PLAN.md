# THE TRIANGLE — REVAMP PLAN

## Revamp target

A complete, replayable **two-environment vertical slice** that represents the final game's quality bar without pretending the eight-environment production game is already finished.

The slice must deliver:

1. A reliable childhood-bedroom opening dream.
2. A deliberate bedroom-to-station transition.
3. A playable Platform 12 sequence with the wrong train.
4. Lucidity, Recognition, manifestation, motif, hum, notebook, false-wake, save, and load loops working as one system.
5. Responsive keyboard/mouse and gamepad input, readable subtitles, safe pause behavior, and no null-HUD crashes.
6. Versioned persistence that restores the complete slice state.
7. A clean UE 5.8.2 build, automation coverage, and GTX 1080-conscious runtime costs.

## Architecture

- `UTriangleGameFlowSubsystem` is the single owner of procedural environment transitions.
- `ATriangleDreamDirector` owns presence, manifestation selection, cooldowns, witnessed Recognition, motifs, and Triangle targets.
- Player-owned progression stays in `ULTriangleLucidityComponent` and `ULTriangleRecognitionComponent`.
- `UTriangleSaveSubsystem` is the only owner of slot I/O and snapshot application. Cross-environment loads are transactional (`Applied`, `Queued`, or `Failed`), reject unknown environment IDs, migrate legacy schema data explicitly, and cancel stale Director/wake callbacks before restoring state.
- World builders are disposable environment factories; all actors they spawn must be lifecycle-owned and cleaned up during a transition.
- C++ UI remains asset-free for the slice, but is responsive and replaceable by a Blueprint widget later.

## Upgrade workstreams

### Foundation and reliability

- Remove stale plugin workarounds and keep the runtime module minimal.
- Validate the installed engine before invoking UnrealBuildTool.
- Add deterministic editor-build and automation-test scripts.
- Add schema-versioned saves and side-effect-free restoration.
- Remove per-frame validity polling and unsafe UI pointer chains.

### Player experience

- Rebuild the HUD around anchors rather than fixed pixel coordinates.
- Queue subtitles, expose subtitle scale/opacity, and synchronize both hidden meters.
- Add gamepad bindings and pause input.
- Make the notebook one-time progression per page.
- Keep interaction prompts focused, safe, and responsive.

### World and flow

- Fix the bedroom corner coordinate so the director can actually watch it.
- Make room and station actors tear down without leaks.
- Give the train real meshes, working doors, and interaction.
- Transition through a black dream cut and move the player to a tagged generated start.
- Keep the station at Platform 12 and make the train motif visibly consequential.

### Dream director

- Replace pool-order assumptions with recognition gates and authored selection weights.
- Enforce each manifestation's cooldown.
- Grant Recognition only when a visible form is actually witnessed.
- Use serial IDs so stale manifestation timers cannot corrupt newer events.
- Rebind safely after environment and player changes.

### Performance and accessibility

- Target SM5/DX12 on the GTX 1080; keep Lumen, Nanite, ray tracing, motion blur, and bloom off by default.
- Avoid unnecessary ticks and excessive dynamic lights.
- Support hidden progression meters, scalable subtitles, and high-contrast prompts.
- Profile the actual slice before restoring more expensive effects.

## Full-game backlog after the vertical slice

The following are intentionally separate production phases, not hidden inside the slice:

1. Clock room and geometry-shift puzzle.
2. Marble mansion with room reauthorization and impossible windows.
3. Castle and the Floating Man's lead behavior.
4. Two witch encounters with contradictory testimony.
5. Layered false-bedroom environment transformations.
6. Fused Platform 12 finale and approach ending.
7. Real materials, meshes, animation, lighting, sound design, and music pass.
8. Packaging, optimization, accessibility QA, controller QA, and content-complete balancing.

## Acceptance gate

The revamp is playable only when a fresh editor build succeeds, all `TheTriangle.Core` automation tests pass, and a PIE smoke test completes: bedroom spawn → notebook → corner manifestation → High Lucidity door → station transition → wrong-train interaction → save → restart → load.
