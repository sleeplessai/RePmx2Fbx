param(
    [Parameter(Mandatory=$true)]
    [string]$ZipPath,
    
    [Parameter(Mandatory=$false)]
    [string]$OutputDir,
    
    [switch]$InPlace
)

$ErrorActionPreference = "Stop"

$zipName = [System.IO.Path]::GetFileNameWithoutExtension($ZipPath)
$zipDir = [System.IO.Path]::GetDirectoryName($ZipPath)

if ($OutputDir) {
    $baseDir = $OutputDir
} else {
    $baseDir = $zipDir
}

$extractDir = Join-Path $baseDir $zipName

if (-not (Test-Path $extractDir)) {
    New-Item -ItemType Directory -Path $extractDir -Force | Out-Null
}

Write-Host "Extracting $ZipPath to $extractDir"
Expand-Archive -Path $ZipPath -DestinationPath $extractDir -Force

function Extract-NestedZips {
    param([string]$ParentDir)
    
    $nestedZips = Get-ChildItem -Path $ParentDir -Filter "*.zip" -Recurse
    
    foreach ($nestedZip in $nestedZips) {
        $nestedName = [System.IO.Path]::GetFileNameWithoutExtension($nestedZip.Name)
        $nestedExtractDir = Join-Path $ParentDir $nestedName
        
        Write-Host "  Extracting nested zip: $($nestedZip.FullName) to $nestedExtractDir"
        
        if (-not (Test-Path $nestedExtractDir)) {
            New-Item -ItemType Directory -Path $nestedExtractDir -Force | Out-Null
        }
        
        Expand-Archive -Path $nestedZip.FullName -DestinationPath $nestedExtractDir -Force
        
        Remove-Item $nestedZip.FullName -Force
        
        Extract-NestedZips -ParentDir $nestedExtractDir
    }
}

Write-Host "Extracting nested zips in $extractDir"
Extract-NestedZips -ParentDir $extractDir

if ($InPlace) {
    Write-Host "In-Place mode: Converting PMX files in $extractDir"
    
    $pmxFiles = Get-ChildItem -Path $extractDir -Filter "*.pmx" -Recurse
    
    $exePath = Join-Path (Split-Path $PSScriptRoot -Parent) "bin\Pmx2Fbx.exe"
    if (-not (Test-Path $exePath)) {
        $exePath = Join-Path (Split-Path $PSScriptRoot -Parent) "build\Pmx2Fbx.exe"
    }
    
    if (-not (Test-Path $exePath)) {
        Write-Error "Pmx2Fbx.exe not found. Please compile from source or obtain release."
        exit 1
    }
    
    foreach ($pmxFile in $pmxFiles) {
        Write-Host "  Converting $($pmxFile.FullName)"
        & $exePath $pmxFile.FullName
    }
    
    Write-Host "In-Place conversion complete. Both .pmx and .pmx.fbx files exist."
} else {
    $pmxDirs = @()
    
    function Find-PmxDirs {
        param([string]$ParentDir)
        
        $items = Get-ChildItem -Path $ParentDir -Directory
        foreach ($item in $items) {
            $hasPmx = Get-ChildItem -Path $item.FullName -Filter "*.pmx" -File
            if ($hasPmx) {
                $pmxDirs += $item.FullName
            }
            Find-PmxDirs -ParentDir $item.FullName
        }
        
        $rootPmx = Get-ChildItem -Path $ParentDir -Filter "*.pmx" -File
        if ($rootPmx -and -not ($pmxDirs -contains $ParentDir)) {
            $pmxDirs += $ParentDir
        }
    }
    
    Find-PmxDirs -ParentDir $extractDir
    
    foreach ($pmxDir in $pmxDirs) {
        $dirName = Split-Path $pmxDir -Leaf
        $fbxDir = $pmxDir
        
        if (-not $dirName.EndsWith("_fbx")) {
            $fbxDir = $pmxDir + "_fbx"
            
            $rootDir = Split-Path $pmxDir -Parent
            if ($rootDir -eq $extractDir) {
                $fbxDir = Join-Path $extractDir ($dirName + "_fbx")
            }
            
            Write-Host "Copying $pmxDir to $fbxDir"
            Copy-Item -Path $pmxDir -Destination $fbxDir -Recurse -Force
            $pmxDir = $fbxDir
        }
        
        Write-Host "Converting PMX files in $pmxDir"
        
        $exePath = Join-Path (Split-Path $PSScriptRoot -Parent) "bin\Pmx2Fbx.exe"
        if (-not (Test-Path $exePath)) {
            $exePath = Join-Path (Split-Path $PSScriptRoot -Parent) "build\Pmx2Fbx.exe"
        }
        
        if (-not (Test-Path $exePath)) {
            Write-Error "Pmx2Fbx.exe not found. Please compile from source or obtain release."
            exit 1
        }
        
        $pmxFiles = Get-ChildItem -Path $pmxDir -Filter "*.pmx" -Recurse
        foreach ($pmxFile in $pmxFiles) {
            Write-Host "  Converting $($pmxFile.FullName)"
            & $exePath $pmxFile.FullName
        }
        
        Get-ChildItem -Path $pmxDir -Filter "*.pmx" -File | Remove-Item -Force
        
        Write-Host "Conversion complete. PMX files removed, only FBX files remain."
    }
}

Write-Host "Done!"