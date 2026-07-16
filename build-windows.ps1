$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $ProjectRoot "build"

Write-Host "Validando Maker Trance..." -ForegroundColor Cyan
python (Join-Path $ProjectRoot "tools\validate_project.py")

if (Test-Path $BuildDir) {
    Remove-Item $BuildDir -Recurse -Force
}

Write-Host "Configurando Visual Studio 2022 x64..." -ForegroundColor Cyan
cmake -S $ProjectRoot -B $BuildDir -G "Visual Studio 17 2022" -A x64

Write-Host "Compilando Maker Trance VST2..." -ForegroundColor Cyan
cmake --build $BuildDir --config Release --target MakerTrance-vst2 --parallel

$dll = Get-ChildItem -Path $BuildDir -Recurse -File -Filter "*.dll" |
    Where-Object { $_.Name -like "*MakerTrance*vst2*.dll" -or $_.Name -like "*MakerTrance*.dll" } |
    Select-Object -First 1

if (-not $dll) {
    throw "A DLL VST2 não foi encontrada após a compilação."
}

$Output = Join-Path $ProjectRoot "Maker Trance by Alza Produz.dll"
Copy-Item $dll.FullName $Output -Force
Write-Host "Pronto: $Output" -ForegroundColor Green
