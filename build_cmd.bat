@echo off
setlocal enabledelayedexpansion

set "UE_DIR=D:\Program Files\UE_5.8\Engine"
set "DOTNET_DIR=D:\Program Files\UE_5.8\Engine\Binaries\ThirdParty\DotNet\10.0\win-x64"
set "PROJECT=C:\Users\natha\projects\the-triangle\Triangle.uproject"
set "MSBUILD=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\amd64\MSBuild.exe"
set "NET_FX=C:\Windows\Microsoft.NET\Framework64\v4.0.30319"

echo === Triangle Build ===
set "PATH=%DOTNET_DIR%;%NET_FX%;%PATH%"
echo Building...
call "%UE_DIR%\Build\BatchFiles\Build.bat" Triangle Win64 Development -Project="%PROJECT%" -NoHotReload -WaitMutex
echo Build result: %ERRORLEVEL%
