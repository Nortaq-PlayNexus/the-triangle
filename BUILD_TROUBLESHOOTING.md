# Build troubleshooting

## Current machine gate (2026-09-24)

The UE 5.8.2 Launcher installation has valid precompiled UBT rule assemblies, but several native editor payloads are missing. UnrealBuildTool reaches module creation and then fails before `Source/Triangle`:

```text
Unable to instantiate module 'TextureFormatOodle'
(referenced via TriangleEditor -> Launch.Build.cs -> ... -> TargetPlatform.Build.cs)
```

`scripts/Test-UnrealInstall.ps1` now checks the precompiled rules assemblies and the native codec payloads explicitly. The manifest's `SourceFiles` entries are build-machine source paths and are intentionally not required in a Launcher installation. The current missing payload set is TextureFormatOodle 2.9.5, 2.9.14, 2.9.15, and 2.9.16. The absent `LC_JumpToSelf.lib` is reported as a non-fatal warning because UBT treats it as an optional dependency. A Launcher Verify/repair is required; do not add or copy engine files by hand.

This is an engine installation problem, not a Triangle source error. Do not edit `D:\Program Files\UE_5.8` or add fake module rules.

Repair steps:

1. Close the Unreal Editor and UnrealBuildTool.
2. In Epic Games Launcher, open **Unreal Engine 5.8**.
3. Choose **Manage → Verify**.
4. Wait for verification/repair to finish, then run:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/Test-UnrealInstall.ps1
```

The check must report complete precompiled rules and native payloads before running the project build:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/Build-Triangle.ps1 -Target Editor
```

The project build script intentionally runs the installation check first so this failure is reported clearly instead of producing a misleading project error.
