[CmdletBinding()]
param(
    [string]$VcpkgRepository = "https://github.com/microsoft/vcpkg.git",
    [string]$Triplet = "x64-windows",
    [switch]$SkipInstall
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$toolsDir = Join-Path $repoRoot "tools"
$vcpkgDir = Join-Path $toolsDir "vcpkg"
$vcpkgExe = Join-Path $vcpkgDir "vcpkg.exe"
$bootstrapScript = Join-Path $vcpkgDir "bootstrap-vcpkg.bat"

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw "git is required to install vcpkg."
}

if (-not (Test-Path -LiteralPath $vcpkgDir)) {
    New-Item -ItemType Directory -Path $toolsDir -Force | Out-Null
    Write-Host "Cloning vcpkg into tools/vcpkg..."
    & git clone $VcpkgRepository $vcpkgDir
} elseif (-not (Test-Path -LiteralPath (Join-Path $vcpkgDir ".git"))) {
    throw "tools/vcpkg exists, but it is not a git clone. Remove it or install vcpkg there manually."
} else {
    Write-Host "vcpkg source already exists at tools/vcpkg."
}

if (-not (Test-Path -LiteralPath $vcpkgExe)) {
    if (-not (Test-Path -LiteralPath $bootstrapScript)) {
        throw "bootstrap-vcpkg.bat was not found in tools/vcpkg."
    }

    Write-Host "Bootstrapping vcpkg..."
    & $bootstrapScript
} else {
    Write-Host "vcpkg executable already exists."
}

if (-not $SkipInstall) {
    Write-Host "Installing manifest dependencies for triplet '$Triplet'..."
    Push-Location $repoRoot
    try {
        & $vcpkgExe install --triplet $Triplet
    } finally {
        Pop-Location
    }
}

Write-Host "vcpkg setup complete."
Write-Host 'Configure with: cmake -S . -B build "-DCMAKE_TOOLCHAIN_FILE=tools/vcpkg/scripts/buildsystems/vcpkg.cmake"'
