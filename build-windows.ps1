$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $ProjectRoot

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw "Git nao encontrado. Use o GitHub Actions incluido no projeto."
}
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    throw "CMake nao encontrado. Use o GitHub Actions incluido no projeto."
}

if (-not (Test-Path ".\dpf\CMakeLists.txt")) {
    git clone --depth 1 --recursive https://github.com/DISTRHO/DPF.git dpf
}

Remove-Item -Recurse -Force .\build -ErrorAction SilentlyContinue
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target MakerTrance-vst2 --parallel

$dll = Get-ChildItem -Path build -Recurse -File -Filter "*.dll" |
    Where-Object { $_.Name -like "*Maker*Trance*.dll" } |
    Select-Object -First 1

if (-not $dll) {
    throw "A DLL VST2 nao foi encontrada."
}

Write-Host "DLL gerada em: $($dll.FullName)"
