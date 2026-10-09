[CmdletBinding()]
param(
    [ValidateSet('Editor', 'Game')]
    [string]$Target = 'Editor',

    [ValidateSet('Debug', 'DebugGame', 'Development', 'Shipping', 'Test')]
    [string]$Configuration = 'Development',

    [string]$EngineRoot = 'D:\Program Files\UE_5.8',

    [switch]$Launch
)

$ErrorActionPreference = 'Stop'
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $ProjectRoot 'Triangle.uproject'
$BuildScript = Join-Path $EngineRoot 'Engine\Build\BatchFiles\Build.bat'

if (-not (Test-Path -LiteralPath $ProjectFile -PathType Leaf)) {
    throw "Project file not found: $ProjectFile"
}

$InstallCheck = Join-Path $PSScriptRoot 'Test-UnrealInstall.ps1'
if (Test-Path -LiteralPath $InstallCheck -PathType Leaf) {
    & $InstallCheck -EngineRoot $EngineRoot
}

if (-not (Test-Path -LiteralPath $BuildScript -PathType Leaf)) {
    throw "Unreal build script not found: $BuildScript. Set -EngineRoot to the UE 5.8 installation directory."
}

$DotNetRoot = Join-Path $EngineRoot 'Engine\Binaries\ThirdParty\DotNet\10.0\win-x64'
if (-not (Test-Path -LiteralPath (Join-Path $DotNetRoot 'dotnet.exe') -PathType Leaf)) {
    throw "Bundled Unreal .NET runtime not found under: $DotNetRoot"
}

$env:DOTNET_ROOT = $DotNetRoot
$env:DOTNET_CLI_TELEMETRY_OPTOUT = '1'
$env:Path = "$DotNetRoot;$env:Path"

$TargetName = if ($Target -eq 'Editor') { 'TriangleEditor' } else { 'Triangle' }
$BuildArguments = @(
    $TargetName,
    'Win64',
    $Configuration,
    "-Project=$ProjectFile",
    '-NoHotReload',
    '-WaitMutex'
)

Write-Host "Building $TargetName $Configuration with UE at $EngineRoot"
& $BuildScript @BuildArguments
if ($LASTEXITCODE -ne 0) {
    throw "UnrealBuildTool failed with exit code $LASTEXITCODE."
}

if (-not $Launch) {
    return
}

if ($Target -eq 'Editor') {
    $Executable = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
    & $Executable $ProjectFile -game
}
else {
    $Executable = Join-Path $ProjectRoot 'Binaries\Win64\Triangle.exe'
    if (-not (Test-Path -LiteralPath $Executable -PathType Leaf)) {
        throw "Game executable not found after build: $Executable"
    }
    & $Executable
}
