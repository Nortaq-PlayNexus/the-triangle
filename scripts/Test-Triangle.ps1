[CmdletBinding()]
param(
    [string]$EngineRoot = 'D:\Program Files\UE_5.8',
    [switch]$SkipBuild
)

$ErrorActionPreference = 'Stop'
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $ProjectRoot 'Triangle.uproject'
$EditorCmd = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$BuildScript = Join-Path $PSScriptRoot 'Build-Triangle.ps1'

if (-not $SkipBuild) {
    & $BuildScript -Target Editor -EngineRoot $EngineRoot
    if ($LASTEXITCODE -ne 0) {
        throw "Editor build failed with exit code $LASTEXITCODE."
    }
}
if (-not (Test-Path -LiteralPath $EditorCmd -PathType Leaf)) {
    throw "UnrealEditor-Cmd not found: $EditorCmd"
}

Write-Host 'Running TheTriangle.Core automation tests...'
& $EditorCmd `
    $ProjectFile `
    -unattended `
    -nopause `
    -NullRHI `
    -NoSound `
    -NoSplash `
    '-ExecCmds=Automation RunTests TheTriangle.Core' `
    '-TestExit=Automation Test Queue Empty' `
    "-ReportExportPath=$(Join-Path $ProjectRoot 'Saved\Automation')"

$exitCode = $LASTEXITCODE
$logPath = Join-Path $ProjectRoot 'Saved\Logs\Triangle.log'
if (Test-Path -LiteralPath $logPath -PathType Leaf) {
    $log = Get-Content -LiteralPath $logPath -Raw
    if ($log -match 'Automation Test Failed|Test Completed. Result={Fail}') {
        throw "One or more automation tests failed. See $logPath"
    }
}
if ($exitCode -ne 0) {
    throw "Automation process failed with exit code $exitCode. See $logPath"
}

Write-Host 'Automation tests passed.'
