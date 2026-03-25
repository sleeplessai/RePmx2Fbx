---
name: mmd-to-fbx
description: Converts MMD PMX models to FBX. Accepts either a directory (scans recursively for .pmx files) or a straight .pmx file path. Provides both PowerShell and Python invocation scripts.
---

# MMD to FBX Conversion Skill (PMX2FBX)

This skill automates the process of converting `.pmx` models into `.fbx` format for use in modern 3D engines.

## Prerequisites

**INSTALLATION REQUIREMENT**: Before using this skill, you must manually copy the `Pmx2Fbx.exe` tool into the skill's local `bin/` directory. This ensures the skill is fully self-contained and ready to be dynamically triggered.
Place the executable here: `\.agents\skills\mmd-to-fbx\bin\Pmx2Fbx.exe`

## Features

1. **Recursive Directory Scanning**: Given a folder path, it will search recursively for all `.pmx` files contained within.
2. **Direct File Target**: Given a specific `.pmx` file path, it will bypass directory scanning and convert directly.
3. **In-Place Conversion**: Automates the invocation of `Pmx2Fbx.exe` against all discovered `.pmx` files, creating matching `.pmx.fbx` models directly alongside their original textures and materials.
4. **Cross-Language Support**: Includes both a PowerShell and Python script for flexible invocation.

## Usage

### Using PowerShell

Execute the included PowerShell script directly. It takes a `-Path` argument pointing to the target directory or `.pmx` file.

```powershell
& "<workspace>\.agents\skills\mmd-to-fbx\scripts\Convert-MmdToFbx.ps1" -Path "C:\path\to\model_folder"
```

### Using Python

Execute the included Python script directly.

```bash
python "<workspace>\.agents\skills\mmd-to-fbx\scripts\convert_mmd_to_fbx.py" "C:\path\to\model_folder"
```
