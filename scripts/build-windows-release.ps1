# Windows release package builder
# Usage: powershell -ExecutionPolicy Bypass -File scripts\build-windows-release.ps1 -Version v1.0.0
param(
    [Parameter(Mandatory=$true)][string]$Version,
    [string]$QtPrefix = "C:\Qt\6.12.0\msvc2022_64",
    [string]$SourceDir = ""
)

$ErrorActionPreference = "Stop"
if ([string]::IsNullOrEmpty($SourceDir)) {
    $SourceDir = Split-Path -Parent $PSScriptRoot
}
$Name = "PeriodicTable-$Version-win64"
$BuildDir = Join-Path $SourceDir "build"
$StageDir = Join-Path $SourceDir "dist\$Name"

Write-Host "==> Building $Name" -ForegroundColor Cyan

Get-Process PeriodicTable -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
Push-Location $SourceDir
cmake --build $BuildDir --config Release 2>&1 | Select-Object -Last 3
Pop-Location

$exe = Join-Path $BuildDir "Release\PeriodicTable.exe"
if (-not (Test-Path $exe)) { throw "Build failed: $exe not found" }

if (Test-Path $StageDir) { Remove-Item $StageDir -Recurse -Force }
New-Item -ItemType Directory -Force -Path $StageDir | Out-Null
Copy-Item $exe $StageDir

$wq = Join-Path $QtPrefix "bin\windeployqt.exe"
if (Test-Path $wq) {
    Write-Host "==> windeployqt" -ForegroundColor Cyan
    & $wq --release --no-translations --no-system-d3d-compiler --no-opengl-sw --no-quick-import (Join-Path $StageDir "PeriodicTable.exe") | Out-Null
} else {
    Write-Warning "windeployqt not found: $wq"
}

# README: build the Chinese text from unicode escapes so this script stays pure ASCII
$sb = New-Object System.Text.StringBuilder
[void]$sb.AppendLine([string]::Join('', [char[]]@(0x5143,0x7D20,0x5468,0x671F,0x8868)) + " Pro $Version (Windows x64)")
[void]$sb.AppendLine("")
[void]$sb.AppendLine([string]::Join('', [char[]]@(0x8FD0,0x884C,0x65B9,0x5F0F,0xFF1A)) + "PeriodicTable.exe")
[void]$sb.AppendLine([string]::Join('', [char[]]@(0x672C,0x76EE,0x5F55,0x5DF2,0x5305,0x542B,0x5168,0x90E8)) + " Qt " + [string]::Join('', [char[]]@(0x8FD0,0x884C,0x5E93,0xFF0C,0x65E0,0x9700,0x5B89,0x88C5,0x4EFB,0x4F55,0x4F9D,0x8D56,0x3002)))
[System.IO.File]::WriteAllText((Join-Path $StageDir "readme.txt"), $sb.ToString(), [System.Text.UTF8Encoding]::new($false))

New-Item -ItemType Directory -Force -Path (Join-Path $SourceDir "dist") | Out-Null
$sevenZip = "C:\Program Files\7-Zip\7z.exe"
if (Test-Path $sevenZip) {
    $outFile = Join-Path $SourceDir "dist\$Name.7z"
    & $sevenZip a -t7z -mx=9 $outFile "$StageDir\*" | Out-Null
} else {
    $outFile = Join-Path $SourceDir "dist\$Name.zip"
    Compress-Archive -Path "$StageDir\*" -DestinationPath $outFile -Force
}

$hash = (Get-FileHash $outFile -Algorithm SHA256).Hash.ToLower()
$sumFile = Join-Path $SourceDir "dist\SHA256SUMS.txt"
"$hash  $(Split-Path -Leaf $outFile)" | Set-Content $sumFile -Encoding ASCII

Write-Host "==> Done" -ForegroundColor Green
Get-Item $outFile | Select-Object Name, @{N='MB';E={[math]::Round($_.Length/1MB,2)}}
Get-Content $sumFile
