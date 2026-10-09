$base = "D:\Program Files\UE_5.8\Engine\Plugins\Chooser\Binaries"
$fails = @()

function Find-Fail {
    param([string]$Path, [int]$Depth)
    if ($Depth -gt 8) { return }
    
    try {
        $dirs = Get-ChildItem $Path -Directory -ErrorAction Stop
        foreach ($d in $dirs) {
            try {
                Get-ChildItem $d.FullName -ErrorAction Stop | Out-Null
                Find-Fail -Path $d.FullName -Depth ($Depth + 1)
            } catch {
                $fails += $d.FullName
                Write-Output "FAIL DIR: $($d.FullName)"
                # Try to list just subdirs
                try {
                    Get-ChildItem $d.FullName -Directory -ErrorAction SilentlyContinue | ForEach-Object {
                        Write-Output "  SUB: $($_.Name)"
                    }
                } catch {}
            }
        }
    } catch {
        $fails += $Path
        Write-Output "FAIL ROOT: $Path"
    }
}

Find-Fail -Path $base -Depth 0
Write-Output "Total failures: $($fails.Count)"
