param(
    [Parameter(Mandatory=$true)]
    [string]$Path
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $Path)) {
    Write-Error "Path does not exist: $Path"
    exit 1
}

# Determine local skill bin directory executable
$exePath = Join-Path $PSScriptRoot "..\bin\Pmx2Fbx.exe"
if (-not (Test-Path -LiteralPath $exePath)) {
    Write-Error "Cannot find converter at: $exePath. Please ensure you copied Pmx2Fbx.exe to the skill's bin folder."
    exit 1
}

$item = Get-Item -LiteralPath $Path

$pmxFiles = @()

if ($item.PSIsContainer) {
    Write-Host "Finding all PMX models in directory: $($item.FullName)"
    $pmxFiles = Get-ChildItem -LiteralPath $item.FullName -Filter "*.pmx" -Recurse
} else {
    if ($item.Extension -eq ".pmx") {
        $pmxFiles = @($item)
    } else {
        Write-Error "Target file is not a PMX file: $($item.FullName)"
        exit 1
    }
}

if ($pmxFiles.Count -eq 0) {
    Write-Host "No PMX files found in target path."
    exit 0
}

Write-Host "Starting in-place conversion from PMX to FBX..."

foreach ($pmxFile in $pmxFiles) {
    Write-Host "Converting: $($pmxFile.FullName)"
    & $exePath $pmxFile.FullName
}

Write-Host "Conversion completed successfully!"
