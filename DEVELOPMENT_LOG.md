# DEVELOPMENT LOG

Session-by-session account of what actually happened.

## 2026-09-14 — Machine inspection, tool installs, project scaffold (THIS SESSION)

- Read `Downloads\new 1.txt` (the THE TRIANGLE design doc) and the OpenCode global state files.
- Inspected machine: i7-8700 / 24 GB / GTX 1080 8 GB / Win10 10.0.19045 / ~1.7 TB free.
- Found **no Unreal Engine, no Epic launcher**, VS Build Tools 2022 missing the C++/SDK workloads, no CMake, no dotnet.
- Decision (user): install Epic Games Launcher + UE 5.x; I do the machine work, user handles the Epic-account engine download.
- Installed **Epic Games Launcher** (1.3.193.0) via `EpicGamesLauncherInstaller.msi` — succeeded (`C:\Program Files\Epic Games\Launcher`).
- Launched VS Build Tools **VCTools (+ Windows 11 SDK 10.0.22621)** workload install — elevated, in progress at close of session.
- Created `C:\Users\natha\projects\the-triangle` (git repo, initial commit `102ec7b`) with full folder map from the design doc.
- Wrote engine-agnostic scaffold: `Triangle.uproject` (EngineAssociation 5.6, plugings: EnhancedInput), Target files, `Triangle.Build.cs`, module files.
- **Wrote the complete Phase-1 C++ foundation** (system list in `DEVELOPMENT_STATUS.md`): gameplay statics, game mode, save system, lucidity, interaction, first-person character (Enhanced Input in pure C++), controller, C++ HUD, Dream Director, Triangle point, procedural bedroom builder, dream door, notebook, audio subsystem, shared entity-state enum. ~26 source files.
- Wrote docs: README, DEVELOPMENT_STATUS, GAME_DESIGN, TECHNICAL_ARCHITECTURE, DREAM_SYSTEM, STORY, LEVEL_DESIGN, AUDIO_DESIGN, PERFORMANCE, ASSET_LICENSES (this log).
- **Not compiled or run yet** — everything is gated on the engine install.

### Next session
1. Confirm VS C++/SDK workload finished (cl.exe + Windows Kits present).
2. Have the user log into the Epic launcher and install Unreal Engine 5.6.x.
3. Revalidate EngineAssociation → generate project files → first compile → fix errors → PIE test movement/interaction/save/lucidity.
4. Commit; update this log + DEVELOPMENT_STATUS.md + remember files.