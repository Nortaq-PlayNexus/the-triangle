$base = "D:\Program Files\UE_5.8\Engine\Plugins"
$fails = @()

function Test-Dir {
    param([string]$Path, [int]$Depth)
    if ($Depth -gt 5) { return }
    try {
        $items = Get-ChildItem $Path -ErrorAction Stop
        foreach ($item in $items) {
            if ($item.PSIsContainer) {
                Test-Dir -Path $item.FullName -Depth ($Depth + 1)
            }
        }
    } catch {
        $fails += @{ Path = $Path; Depth = $Depth; Error = $_.Exception.Message }
    }
}

Test-Dir -Path $base -Depth 0
$fails | ForEach-Object { Write-Output "FAIL at depth $($_.Depth): $($_.Path) - $($_.Error)" }
Write-Output "Total failures: $($fails.Count)"
