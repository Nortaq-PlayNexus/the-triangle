@echo off
setlocal enabledelayedexpansion

set "UE_DIR=D:\Program Files\UE_5.8\Engine"
set "DOTNET_DIR=D:\Program Files\UE_5.8\Engine\Binaries\ThirdParty\DotNet\10.0\win-x64"
set "PROJECT=C:\Users\natha\projects\the-triangle\Triangle.uproject"

echo === Triangle Build (Precompiled Rules) ===
set "PATH=%DOTNET_DIR%;%PATH%"

"%UE_DIR%\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" Triangle Win64 Development -Project="%PROJECT%" -NoHotReload -WaitMutex -bUsePrecompiled -bUseShippingRuntime 2>&1
