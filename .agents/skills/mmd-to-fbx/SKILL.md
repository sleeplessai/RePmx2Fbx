---
name: mmd-to-fbx
description: Convert MMD (Mikaku Miku Dance) model folders containing .pmx files and their assets to FBX format. Use this skill whenever the user wants to convert MMD models to FBX, whether they mention it casually ("convert my mmd model"), provide a folder path, or explicitly ask about MMD-to-FBX conversion. This skill handles the full workflow: finding .pmx files in a folder, running the conversion tool on each, and organizing the output.
---

# MMD to FBX Conversion Skill

This skill converts MMD (Mikaku Miku Dance) model folders to FBX format using the built `Pmx2Fbx.exe` converter.

## Requirements

- **PowerShell 5.1+** (Windows built-in) or **PowerShell 7+** (cross-platform)
- **Pmx2Fbx.exe** - The converter executable:
  - First looks in skill's `bin/` directory
  - Falls back to `<workspace>/build/Pmx2Fbx.exe`
  - If not found, compile from source (`PmxLib` directory) or download release

## Input

- **MMD source**: Either a folder `<mmdmodel>` containing .pmx model files and assets, OR a `.zip` file of an MMD model

## Note

Before running the conversion, ensure that the `Pmx2Fbx.exe` executable is available.
The skill will first look for the executable in its own `bin/` directory (relative to this skill).
If not found there, it will check `<workspace>/build/Pmx2Fbx.exe`.
If the executable is not found in either location, you must compile the source code (located in the `PmxLib` directory) or obtain a release version.

The skill uses PowerShell's `Expand-Archive` command to extract zip files.

## Usage

Use the provided PowerShell script for conversion:

```powershell
# In-Place mode (keep both .pmx and .pmx.fbx files)
& "<skill_path>/scripts/Convert-MmdToFbx.ps1" -ZipPath "<path_to_zip>" -InPlace

# Default mode (create _fbx folders, remove original PMX)
& "<skill_path>/scripts/Convert-MmdToFbx.ps1" -ZipPath "<path_to_zip>"
```

## Output

Depending on the handling option chosen:

**Option 1: Create Separate _fbx Folder (Default)**
- **FBX folder**: A new folder `<mmdmodel>_fbx` containing the converted .fbx files with all assets preserved
- Original files remain unchanged in the source location

**Option 2: In-Place Conversion**
- **Output files**: Converted .fbx files (.pmx.fbx) in the same directory as the source .pmx files
- All assets remain in their original locations alongside both .pmx and .pmx.fbx files

## Conversion Process

### Handling Options

There are two ways to handle the conversion process. The user must specify which option to use when requesting the conversion.

**Option 1: Create Separate _fbx Folder (Default)**
This approach preserves the original files and creates a separate folder for FBX outputs.

**Option 2: In-Place Conversion**
This approach converts files in the original location, keeping both PMX and FBX files together in the same directory.

### If Input is a Zip File

1. **Extract zip**: Extract the `.zip` file to a folder named after the zip file (without .zip extension) using PowerShell's `Expand-Archive` command
2. **Extract nested zips**: Find all *.zip files within the extracted folder and extract each to its own folder named after the nested zip file (without .zip extension)
3. **Process contents**:
   - Scan the extracted folder and all its subdirectories for folders containing .pmx files
   - For each folder found that contains .pmx files:
     * **Option 1 (Default)**: If the folder is not already named with `_fbx` suffix, rename it to `<folder_name>_fbx` and proceed with conversion steps below
     * **Option 2 (In-Place)**: Use the folder as-is for conversion (no renaming)
   - If no folders with .pmx files are found but the root extracted folder contains .pmx files:
     * **Option 1 (Default)**: Rename the extracted folder to `<zip_name>_fbx` (where zip_name is the zip filename without extension) and proceed with conversion steps below
     * **Option 2 (In-Place)**: Use the extracted folder as-is for conversion (no renaming)

### If Input is a Folder

1. **Validate input**: Confirm the MMD folder exists and contains at least one .pmx file
2. **Choose handling method**:
     * **Option 1 (Default)**: Copy the entire `<mmdmodel>` folder to `<mmdmodel>_fbx` - this preserves all assets (textures, materials, etc.)
     * **Option 2 (In-Place)**: Use the folder as-is for conversion (no copying)

### Conversion (Both Cases)

3. **Find .pmx files**: 
     * **Option 1 (Default)**: Recursively locate all .pmx files in the `_fbx` folder
     * **Option 2 (In-Place)**: Recursively locate all .pmx files in the source folder
4. **Convert each .pmx**: For each .pmx file:
    - Run `Pmx2Fbx.exe` via PowerShell with the full path to the .pmx file
    - First look for the executable in the skill's bin directory, then fall back to the workspace build directory:
      ```powershell
      $exePath = Join-Path (Split-Path $PSScriptRoot -Parent) "bin\Pmx2Fbx.exe"
      if (-Not (Test-Path $exePath)) {
          $exePath = "<workspace>/build/Pmx2Fbx.exe"
      }
      & "$exePath" "<full_path_to_pmx_file>"
      ```
    - The tool creates output with `.pmx.fbx` extension in the same directory as the source
5. **Handle original files**:
     * **Option 1 (Default)**: Delete the original `.pmx` files from the `_fbx` folder, keeping the `.pmx.fbx` files and all assets
     * **Option 2 (In-Place)**: Keep both the original `.pmx` files and the new `.pmx.fbx` files, along with all assets

## PowerShell Execution

Always use PowerShell to execute the conversion. First look for the executable in the skill's bin directory, then fall back to the workspace build directory:

```powershell
$exePath = Join-Path (Split-Path $PSScriptRoot -Parent) "bin\Pmx2Fbx.exe"
if (-Not (Test-Path $exePath)) {
    $exePath = "<workspace>/build/Pmx2Fbx.exe"
}
& "$exePath" "<full_path_to_pmx_file>"
```

## Example

Given input folder: `C:\Users\Someone\Models\MyMiku`
- Contains: `MyMiku.pmx`, `tex\skin.png`, `tex\eye.png`, `spa\hair.bmp`

**Option 1 (Default - Separate Folder):**
- Output folder: `C:\Users\Someone\Models\MyMiku_fbx`
- Output files: `MyMiku.pmx.fbx`, `tex\skin.png`, `tex\eye.png`, `spa\hair.bmp` (all original assets preserved)
- Original folder unchanged: `C:\Users\Someone\Models\MyMiku\` still contains `MyMiku.pmx`, `tex\skin.png`, etc.

**Option 2 (In-Place Conversion):**
- Output files: `C:\Users\Someone\Models\MyMiku\MyMiku.pmx.fbx`, `tex\skin.png`, `tex\eye.png`, `spa\hair.bmp`
- Both `.pmx` and `.pmx.fbx` files exist in the same directory alongside all assets

Given input zip: `C:\Downloads\MikuCollection.zip` (containing multiple model folders like `Miku/`, `Rin/`, `Len/` each with .pmx files and assets)

**Option 1 (Default - Separate Folders):**
- Output folders: `C:\Downloads\MikuCollection_fbx\Miku_fbx\`, `C:\Downloads\MikuCollection_fbx\Rin_fbx\`, `C:\Downloads\MikuCollection_fbx\Len_fbx\`
- Output files: Each model folder contains its `.pmx.fbx` files with all assets preserved
- Original zip-extracted folders remain unchanged

**Option 2 (In-Place Conversion):**
- Output files: Each model folder contains both original `.pmx` files and new `.pmx.fbx` files with all assets preserved
- Example: `C:\Downloads\MikuCollection\Miku\MyMiku.pmx` and `MyMiku.pmx.fbx` alongside `tex\skin.png`, etc.

## Error Handling

- If no .pmx files found in the input folder, report error and stop
- If Pmx2Fbx.exe fails on a file, log the error and continue with remaining files
- Report summary of successful/failed conversions
