# Installs the Visual Studio C++ toolchain (MSVC + Windows 11 SDK) required by
# Unreal Engine, by extending the existing Visual Studio Build Tools 2022
# instance. Run this from an ADMIN PowerShell:
#   right-click "install-vs-toolchain.ps1" -> Run with PowerShell
#   (or: pwsh -ExecutionPolicy Bypass -File .\install-vs-toolchain.ps1 -Admin)
#
# Marker after completion: C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC

param(
    [switch]$Admin
)

$ErrorActionPreference = 'Stop'

$installPath = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools'
$bootstrapper = "$env:TEMP\opencode\vs_BuildTools.exe"
$log = "$env:TEMP\opencode\vs_modify.log"

$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    if (-not $Admin) {
        Write-Host "Not admin. Relaunching elevated for the UAC prompt..." -ForegroundColor Yellow
        Start-Process -Verb RunAs powershell -ArgumentList "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`" -Admin"
        exit 0
    }
    Write-Host "This still requires elevation. Open PowerShell as Administrator and re-run." -ForegroundColor Red
    exit 1
}

if (-not (Test-Path $bootstrapper)) {
    Write-Host "Downloading VS Build Tools bootstrapper..." -ForegroundColor Cyan
    Invoke-WebRequest -Uri 'https://aka.ms/vs/17/release/vs_BuildTools.exe' -OutFile $bootstrapper -UseBasicParsing
}

Write-Host "Installing MSVC + Windows 11 SDK workload. This downloads several GB and can take 10-30 min." -ForegroundColor Cyan
& $bootstrapper modify --installPath $installPath `
    --add Microsoft.VisualStudio.Workload.VCTools `
    --includeRecommended `
    --add Microsoft.VisualStudio.Component.Windows11SDK.22621 `
    --quiet --norestart --wait `
    --log $log

$exit = $LASTEXITCODE
if ($exit -ne 0) {
    Write-Host "Bootstrapper exited with code $exit (see $log)." -ForegroundColor Red
    exit $exit
}

$msvc = Test-Path (Join-Path $installPath 'VC\Tools\MSVC')
$sdk = Test-Path 'C:\Program Files (x86)\Windows Kits\10\Include'
Write-Host "MSVC present: $msvc | Windows SDK present: $sdk" -ForegroundColor Green
if ($msvc -and $sdk) {
    Write-Host "Toolchain ready. THE TRIANGLE can compile." -ForegroundColor Green
} else {
    Write-Host "Toolchain incomplete - check components." -ForegroundColor Red
}