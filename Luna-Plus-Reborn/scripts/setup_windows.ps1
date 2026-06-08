<#
.SYNOPSIS
    Luna Plus Reborn — Windows Build Setup
.DESCRIPTION
    Installs vcpkg + dependencies and configures CMake for Windows.
    Run this from the repository root.
#>

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $RepoRoot "build"
$VcpkgDir = Join-Path $RepoRoot "vcpkg"

Write-Host "=== Luna Plus Reborn — Windows Setup ===" -ForegroundColor Cyan
Write-Host "Repo root: $RepoRoot" -ForegroundColor Gray

# ── Step 1: Check prerequisites ──────────────────────────────────────────────
function Check-Command($cmd) {
    try { Get-Command $cmd -ErrorAction Stop >$null; return $true } catch { return $false }
}

if (!(Check-Command "cmake")) {
    Write-Host "ERROR: CMake not found. Install from https://cmake.org/download/" -ForegroundColor Red
    exit 1
}
Write-Host "[OK] CMake: $(cmake --version | Select-Object -First 1)" -ForegroundColor Green

if (!(Check-Command "git")) {
    Write-Host "ERROR: Git not found. Install from https://git-scm.com/" -ForegroundColor Red
    exit 1
}
Write-Host "[OK] Git: $(git --version)" -ForegroundColor Green

# ── Step 2: Install vcpkg ────────────────────────────────────────────────────
if (!(Test-Path $VcpkgDir)) {
    Write-Host "`n=== Installing vcpkg ===" -ForegroundColor Yellow
    git clone https://github.com/microsoft/vcpkg.git $VcpkgDir
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Failed to clone vcpkg" -ForegroundColor Red
        exit 1
    }
    & "$VcpkgDir\bootstrap-vcpkg.bat"
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: vcpkg bootstrap failed" -ForegroundColor Red
        exit 1
    }
    Write-Host "[OK] vcpkg installed at $VcpkgDir" -ForegroundColor Green
} else {
    Write-Host "[OK] vcpkg already present at $VcpkgDir" -ForegroundColor Green
}

# ── Step 3: Install dependencies ─────────────────────────────────────────────
Write-Host "`n=== Installing dependencies via vcpkg ===" -ForegroundColor Yellow

$Deps = @(
    "bgfx"
    "glfw3"
    "glm"
    "spdlog"
    "fmt"
    "sqlite3"
    "freetype"
    "entt"
    "flatbuffers"
    "assimp"
    "stb"
    "imgui"
)

foreach ($dep in $Deps) {
    Write-Host "  Installing $dep..." -ForegroundColor Gray
    & "$VcpkgDir\vcpkg" install $dep --triplet x64-windows
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Failed to install $dep" -ForegroundColor Red
        exit 1
    }
}
Write-Host "[OK] All dependencies installed" -ForegroundColor Green

# ── Step 4: Configure CMake for Windows ──────────────────────────────────────
Write-Host "`n=== Configuring CMake ===" -ForegroundColor Yellow

if (!(Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
}

$Generator = "Visual Studio 17 2022"
$Arch = "x64"

Push-Location $BuildDir
try {
    cmake $RepoRoot `
        -G "$Generator" `
        -A $Arch `
        -DCMAKE_TOOLCHAIN_FILE="$VcpkgDir\scripts\buildsystems\vcpkg.cmake" `
        -DLUNA_RENDERER="D3D12" `
        -DCMAKE_BUILD_TYPE="Release"

    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: CMake configuration failed" -ForegroundColor Red
        exit 1
    }

    Write-Host "[OK] CMake configured for Windows (x64, D3D12)" -ForegroundColor Green
} finally {
    Pop-Location
}

# ── Step 5: Build instructions ───────────────────────────────────────────────
Write-Host "`n=== Build Instructions ===" -ForegroundColor Cyan
Write-Host ""
Write-Host "  Open the solution:" -ForegroundColor White
Write-Host "    $BuildDir\LunaPlusReborn.sln" -ForegroundColor Yellow
Write-Host ""
Write-Host "  Or build from command line:" -ForegroundColor White
Write-Host "    cmake --build $BuildDir --config Release" -ForegroundColor Yellow
Write-Host ""
Write-Host "  Release binary:" -ForegroundColor White
Write-Host "    $BuildDir\bin\Release\LunaPlusClient.exe" -ForegroundColor Yellow
Write-Host ""
Write-Host "=== Setup complete ===" -ForegroundColor Cyan
