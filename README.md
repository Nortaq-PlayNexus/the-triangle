# THE TRIANGLE

A first-person psychological horror game about a sequence of interconnected nightmares, a recurring geometric presence, lucid dreaming, false awakenings and impossible architecture.

**Everything in this game is fiction.** The horror lives entirely inside the game's fictional universe; no supernatural concept here is presented as real-world fact.

## Status

Pre-alpha. C++ foundation for the first playable slice is written; engine install is the current gate. See `DEVELOPMENT_STATUS.md`.

## GDD synopsis

- The player explores dream environments that initially feel unrelated: a childhood bedroom, an underground train station, a giant clock room, a marble mansion, a castle, witch encounters and layered false awakenings.
- A geometric presence, **The Triangle**, appears in glimpses — never a conventional monster, never fully explained. It may be supernatural, psychological or simply the dream awakening itself.
- A **lucidity** system tracks how aware the player is of the dream; high lucidity lets the player bend dream rules.
- **The Floating Man** does not chase the player — he leads them.
- Everything eventually bleeds together into one merged dream, ending on a question rather than an answer.

## Repository layout

```
Source/Triangle/
  Core/        gameplay statics, game mode
  Characters/  first-person pawn + controller
  Interaction/ interactable component
  Lucidity/    lucidity meter + levels
  Dream/       Dream Director, Triangle manifestations
  Nightmare/   (future) nightmare entities
  Audio/       dream audio subsystem
  Save/        save game + subsystem
  UI/          HUD (C++, no assets)
  AI/          shared entity states
  World/       procedural bedroom, door, notebook
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

## Running

1. Install Unreal Engine 5.6.x via the Epic Games Launcher.
2. Open `Triangle.uproject` (right-click → Generate Visual Studio project files).
3. Build in the editor, then press Play (PIE) to enter the childhood bedroom.
4. Controls: WASD move · mouse look · Space jump · F interact · F5 save · F9 load.