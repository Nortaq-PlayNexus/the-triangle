@echo off
setlocal enabledelayedexpansion

set "UE_DIR=D:\Program Files\UE_5.8\Engine"
set "DOTNET_DIR=D:\Program Files\UE_5.8\Engine\Binaries\ThirdParty\DotNet\10.0\win-x64"
set "PROJECT=C:\Users\natha\projects\the-triangle\Triangle.uproject"
set "PROJECT_DIR=C:\Users\natha\projects\the-triangle"
set "MSBUILD=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\amd64\MSBuild.exe"
set "NET_FX=C:\Windows\Microsoft.NET\Framework64\v4.0.30319"

echo === Triangle Build & Run ===
echo.

echo [1/3] Setting environment...
set "DOTNET_ROOT=%DOTNET_DIR%"
set "PATH=%DOTNET_DIR%;%MSBUILD%;%NET_FX%;%PATH%"
echo DOTNET_ROOT=%DOTNET_DIR%
echo.

echo [2/3] Generating project files...
"%UE_DIR%\Binaries\Win64\UnrealEditor.exe" "%PROJECT%" -projectfiles -WaitMutex -NoHotReload
if %ERRORLEVEL% neq 0 (
    echo WARNING: Project file generation returned error %ERRORLEVEL%
)
echo.

echo [3/3] Building Triangle (Development, Win64)...
"%UE_DIR%\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" Triangle Win64 Development -Project="%PROJECT%" -NoHotReload -WaitMutex
if %ERRORLEVEL% neq 0 (
    echo BUILD FAILED with error code %ERRORLEVEL%
    pause
    exit /b %ERRORLEVEL%
)
echo.

echo === Build complete! ===
echo Launching Triangle.exe...
"%PROJECT_DIR%\Binaries\Win64\Triangle.exe"

endlocal