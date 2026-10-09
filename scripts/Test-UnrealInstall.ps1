[CmdletBinding()]
param(
    [string]$EngineRoot = 'D:\Program Files\UE_5.8'
)

$ErrorActionPreference = 'Stop'
$EngineDirectory = Join-Path $EngineRoot 'Engine'
$ManifestPath = Join-Path $EngineDirectory 'Intermediate\Build\BuildRules\UE5RulesManifest.json'
$RulesAssemblyPath = Join-Path $EngineDirectory 'Intermediate\Build\BuildRules\UE5Rules.dll'
$ProgramRulesAssemblyPath = Join-Path $EngineDirectory 'Intermediate\Build\BuildRules\UE5ProgramRules.dll'
$VersionPath = Join-Path $EngineDirectory 'Build\Build.version'

foreach ($Required in @(
    (Join-Path $EngineDirectory 'Build\BatchFiles\Build.bat'),
    (Join-Path $EngineDirectory 'Binaries\Win64\UnrealEditor-Cmd.exe'),
    $ManifestPath,
    $RulesAssemblyPath,
    $ProgramRulesAssemblyPath,
    $VersionPath
)) {
    if (-not (Test-Path -LiteralPath $Required -PathType Leaf)) {
        throw "Unreal installation is incomplete; missing: $Required"
    }
}

foreach ($RulesAssembly in @($RulesAssemblyPath, $ProgramRulesAssemblyPath)) {
    if ((Get-Item -LiteralPath $RulesAssembly).Length -le 0) {
        throw "Unreal precompiled build rules are empty: $RulesAssembly"
    }
}

$version = Get-Content -LiteralPath $VersionPath -Raw | ConvertFrom-Json
Write-Host "UE $($version.MajorVersion).$($version.MinorVersion).$($version.PatchVersion) ($($version.BranchName))"

# Launcher builds intentionally ship precompiled rules assemblies. The
# manifest's SourceFiles are build-machine source paths, not files that must be
# copied into a binary installation, so checking every manifest path would
# produce a false-positive "missing 465 files" diagnosis.

# The precompiled rules assemblies do not cover every native redistributable
# that UBT instantiates while creating the editor target. Check the Oodle codec
# payload explicitly so a partial Launcher repair cannot be reported healthy.
$missingRuntime = New-Object System.Collections.Generic.List[string]
$oodleRoot = Join-Path $EngineDirectory 'Source\Developer\TextureFormatOodle\Sdks'
foreach ($oodleVersion in @('2.9.5', '2.9.6', '2.9.7', '2.9.8', '2.9.9', '2.9.10', '2.9.11', '2.9.12', '2.9.13', '2.9.14', '2.9.15', '2.9.16')) {
    $oodle = Join-Path $oodleRoot "$oodleVersion\redist\Win64\oo2tex_win64_$oodleVersion.dll"
    if (-not (Test-Path -LiteralPath $oodle -PathType Leaf)) {
        $missingRuntime.Add("TextureFormatOodle\$oodleVersion\redist\Win64\oo2tex_win64_$oodleVersion.dll")
    }
}
$liveCodingLibrary = Join-Path $EngineDirectory 'Source\Developer\Windows\LiveCoding\Private\External\LC_JumpToSelf.lib'
if (-not (Test-Path -LiteralPath $liveCodingLibrary -PathType Leaf)) {
    Write-Host 'Warning: optional Live Coding helper library is absent; UBT may emit a non-fatal dependency warning.' -ForegroundColor Yellow
}
if ($missingRuntime.Count -gt 0) {
    Write-Host "Missing $($missingRuntime.Count) native engine payloads referenced by the installed build:" -ForegroundColor Red
    $missingRuntime | Select-Object -First 20 | ForEach-Object { Write-Host "  $_" -ForegroundColor Red }
    throw 'The Epic Launcher installation has incomplete native codec redistributables. Run another UE 5.8 Manage -> Verify operation; do not patch the engine directory by hand.'
}

Write-Host 'Unreal engine rules and native build payloads are complete.' -ForegroundColor Green
