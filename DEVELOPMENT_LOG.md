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
3. Revalidate EngineAssociation —> generate project files —> first compile —> fix errors —> PIE test movement/interaction/save/lucidity.
4. Commit; update this log + DEVELOPMENT_STATUS.md + remember files.

## 2026-09-14 — Design-spec follow-up integrated (recognition, hum, manifestation/intrusion systems)

The writer appended a major follow-up to `new 1.txt` (lines ~1211-1669): The Triangle is an intrusion, not a monster; the hum; no single body; "cutting into the dream" effects; a full Recognition system; dream realism; the human form; the clock and platform twelve; lucidity ≠ invincibility; the unreliable wake; the Floating Man (reserved); the "how does the player recognise it" narrative principle; an atmospheric rule of less-is-more.

Integrated into the existing architecture as real systems (all written, none compiled yet — still gated on the engine):

- `Lucidity/ULTriangleRecognitionComponent` — separate hidden Recognition stat (0..100, Unknowing/Noticing/Recognizing/Anticipating/Master), deduplicated pattern-catalogue of witnessed manifestation ids, pre-visual "it's here" sensing.
- `Dream/UTriangleManifestationDefinition` (DataAsset) + `Dream/ETriangleManifestationKind` (Point, HumOnly, TriangularGap, CornerSplit, DistantBlackPoint, TriangularDoorway, HumanForm-reserved).
- `Nightmare/ATriangleAnomalyActor` + `UTriangleAnomalySubsystem` — the "cuts into the dream": razor gaps, corner splits, distant black points, impossible doorway triangles; scale-fade in/out, concurrent budget, despawn-on-reset.
- `Audio/ATriangleProceduralHumSource` — the hum, synthesised entirely at runtime (two detuned partials ~43/47 Hz breathing), zero audio assets; `UTriangleAudioSubsystem::SetHumPresence` drives it.
- `Core/UTriangleMotifSubsystem` — dream clock + recurring motifs (ClockStops12, Platform12Train) with cooldowns and recognition gates.
- `Dream/UTriangleFalseAwakeningManager` — recognition-gated wake (Q), layered difference lines, `OnWakeResetRequested`.
- `Dream/ATriangleDreamDirector` — rebuilt: 0..1 presence ramp with Start/Increase/Disappear events, hum following, weighted manifestation pool, Master-sense hum-only appearances, motif hooks, wake withdrawal, anomaly spawning.
- Wired through `ATriangleCharacter` (Recognition component, Q = wake), save (Recognition member), HUD (optional recognition meter), `FTriangleGameplayStatics::GetPlayerRecognition`.

Docs updated: DREAM_SYSTEM, TECHNICAL_ARCHITECTURE, DEVELOPMENT_STATUS.

### Next session
1. Confirm VS C++/SDK workload finished (cl.exe + Windows Kits present).
2. User logs into the Epic launcher and installs Unreal Engine 5.6.x (download reportedly started).
3. Revalidate EngineAssociation —> generate project files —> first compile —> fix errors —> PIE test movement/interaction/save/lucidity/recognition/hum/wake.
4. Commit; update this log + DEVELOPMENT_STATUS.md + remember files.
## 2026-09-14 (eve) — Toolchain + first successful compile on UE 5.8.2
- Toolchain: VS Build Tools 17.14 C++ workload (MSVC 14.44.35207) + Windows SDK 10.0.26100.0 installed via the VS Installer GUI (bootstrapper/engine command lines fail silently on this box). .NET Framework 4.8 SDK added for UBT SwarmInterface.
- Engine upgraded target: UE 5.8.2 at D:\Program Files\UE_5.8 (user installed 5.8, not 5.6). Triangle.uproject EngineAssociation set to "5.8".
- Targets moved to BuildSettingsVersion.V7; Build.cs adds ModuleDirectory to PublicIncludePaths.
- First TriangleEditor build SUCCEEDED after fixing 5.8 API breaks (Enum.h->Class.h, EnhancedInput Context removal, AudioComponent AdjustVolume, USoundWaveProcedural GetSampleRate/GetNumChannels removal, auto* range-for over TObjectPtr, GameMode DefaultPlayerStart removal, light component casts/includes, SpawnActor FRotator overload removal).
- Next: editor launch + first PIE runtime test.

## 2026-09-14 — Post-compile cleanup
- Committed 14 files of post-compile fixes (15 insertions, 40 deletions):
  - RoomBuilder + Character spawn offset to RoomElevation (5000) to avoid template terrain collision
  - Removed dead ApplyClamped() helpers (clamping done inline in SetLucidity/SetRecognition)
  - Removed unused Index variable in DreamDirector pool init
  - Removed unused mesh cache in AnomalyActor; reordered includes
  - Door ClosedRotation moved from ctor to BeginPlay (correct timing before BeginPlay runs)
  - Removed unused DefaultConfig member in MotifSubsystem
  - ManifestationPool changed from EditDefaultsOnly to VisibleAnywhere for debugging
  - DefaultEngine.ini: removed template OpenWorld map references
- Phase 1 marked complete (compiled). Phase 2 (underground station) in progress.

## 2026-09-14 — Phase 2: Underground station builder
- Added `Source/Triangle/World/ETriangleStationArea.h` — station area enum (Platform, Track, Concourse, Tunnel) for builder region routing.
- Added `Source/Triangle/World/ATriangleStationBuilder.h/cpp` — procedural underground station from BasicShapes: shell, two platforms, track bed with sleepers, fluorescent ceiling strips (cold light), ventilation ducts, waiting benches, amber safety lights, Platform 12 sign marker, waiting benches, station fog. Spawns at +5000 elevation for asset-free PIE. Player spawn on right platform facing the tracks.
- Added `Source/Triangle/World/ATriangleTrain.h/cpp` — wrong train (BasicShapes charcoal body, dark windows, sickly interior light; doors + interior light toggle). Not yet animated or Director-integrated.
- Station includes its own corner Triangle point (far platform corner) so the Director has a manifestation target in the station.
- Docs updated: DEVELOPMENT_STATUS.md (Phase 2 in progress), DEVELOPMENT_LOG.md, LEVEL_DESIGN.md (Phase 2 status).
- Next: integrate station into Director's environment cycle; add train manifestation; PIE test bedroom; fix compile issues in station builder.
