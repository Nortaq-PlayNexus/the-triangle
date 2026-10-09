$env:DotnetDir = "D:\Program Files\UE_5.8\Engine\Binaries\ThirdParty\DotNet\10.0\win-x64"
$env:Path = "$env:DotnetDir;C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\amd64;C:\Windows\Microsoft.NET\Framework64\v4.0.30319;$env:Path"
& "D:\Program Files\UE_5.8\Engine\Build\BatchFiles\Build.bat" "Triangle" "Win64" "Development" "-Project=C:\Users\natha\projects\the-triangle\Triangle.uproject" "-NoHotReload" "-WaitMutex" "2>&1" | Out-File -FilePath "C:\Users\natha\projects\the-triangle\build_output.txt" -Encoding UTF8
