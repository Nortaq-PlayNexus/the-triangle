# THE TRIANGLE

A first-person psychological horror game about a sequence of interconnected nightmares, a recurring geometric presence, lucid dreaming, false awakenings and impossible architecture.

**Everything in this game is fiction.** The horror lives entirely inside the game's fictional universe; no supernatural concept here is presented as real-world fact.

## Status

Pre-alpha vertical-slice revamp in progress. The native gameplay foundation targets Unreal Engine **5.8.2** and includes the bedroom, station flow, transactional save/load, responsive HUD/pause shell, and automated core tests. The source pass is complete for this slice, but runtime claims remain blocked: the current UE installation checker reports four missing TextureFormatOodle redistributables. See `DEVELOPMENT_STATUS.md` and `REVAMP_PLAN.md` for the current gate and remaining production phases.

## GDD synopsis

- The player explores dream environments that initially feel unrelated: a childhood bedroom, an underground train station, a giant clock room, a marble mansion, a castle, witch encounters and layered false awakenings.
- A geometric presence, **The Triangle**, appears in glimpses — never a conventional monster, never fully explained. It may be supernatural, psychological or simply the dream awakening itself.
- A **lucidity** system tracks how aware the player is of the dream; high lucidity lets the player bend dream rules.
- **The Floating Man** does not chase the player — he leads them.
- Everything eventually bleeds together into one merged dream, ending on a question rather than an answer.

## Repository layout

```
Source/Triangle/
  Core/        gameplay statics, game mode, environment flow
  Characters/  first-person pawn + controller
  Interaction/ interactable component
  Lucidity/    lucidity/recognition meters + levels
  Dream/       Dream Director, Triangle manifestations, false wakes
  Nightmare/   anomaly actors and intrusion budget
  Audio/       dream audio routing + procedural hum
  Save/        versioned save game + subsystem
  UI/          responsive C++ HUD and pause/accessibility shell
  AI/          shared entity states
  World/       procedural bedroom, station, train, door, notebook
  Tests/       native automation coverage
Content/       art/content lands here (built-in assets, generated materials)
Docs/          design references
```

## Docs

- `GAME_DESIGN.md` — systems, gameplay, atmosphere targets
- `TECHNICAL_ARCHITECTURE.md` — modules, system breakdown, decisions
- `DREAM_SYSTEM.md` — lucidity, wake mechanic, false awakenings, Dream Director
- `STORY.md` — narrative spine and ambiguity rules
- `LEVEL_DESIGN.md` — environments and transitions
- `AUDIO_DESIGN.md` — audio approach and motifs
- `PERFORMANCE.md` — hardware targets and optimisation rules
- `ASSET_LICENSES.md` — provenance of every external asset
- `DEVELOPMENT_LOG.md` — what actually happened, session by session
- `DEVELOPMENT_STATUS.md` — phase progress and current gates
- `REVAMP_PLAN.md` — vertical-slice target, architecture, acceptance gate, and full-game backlog
- `BUILD_TROUBLESHOOTING.md` — UE installation verification and current build gate

## Running

1. Install or repair Unreal Engine **5.8.2** via the Epic Games Launcher.
2. Verify the engine installation: `powershell -ExecutionPolicy Bypass -File scripts/Test-UnrealInstall.ps1`.
3. Build the editor target: `powershell -ExecutionPolicy Bypass -File scripts/Build-Triangle.ps1 -Target Editor -Launch`.
4. Run the core automation tests: `powershell -ExecutionPolicy Bypass -File scripts/Test-Triangle.ps1`.
5. Alternatively, open `Triangle.uproject` and use Play (PIE). The GameMode builds the childhood bedroom procedurally.

Keyboard controls: WASD move · mouse look · Space jump · F interact · Q wake · F5 save · F9 load · Esc pause.

Gamepad controls: left stick move · right stick look · A jump · X interact · B wake · LB save · RB load · Start/Options pause.