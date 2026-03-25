# Pmx2Fbx Documentation & Usage Guide

This document provides a comprehensive overview of how the modernized Pmx2Fbx tool works internally, its technical architecture, and detailed usage instructions.

## 1. Technical Modernization

This fork has been completely refactored to meet modern C++ standards and high-quality software engineering practices:

*   **Modern C++20**: The codebase leverages C++20 features for improved safety and performance.
*   **Full RAII Architecture**: Resource management is automated using smart pointers.
    *   **`FbxHelper::Shape`**: Mesh and node lifetimes are managed by `std::unique_ptr`. Finalization logic (bind pose storage) is automatically triggered via destructors.
    *   **Morph Data**: Morph lists in `PmxReader` now use `std::vector<std::unique_ptr<PmxBaseMorph>>`, eliminating memory leaks inherent in the original design.
*   **Robust File I/O**: Replaced legacy C-style `FILE*` and `malloc` with modern `std::ifstream` and `std::vector<char>` buffers.
*   **Clean String Handling**: Standardized on modern formatting using `std::to_string` and `std::stringstream` with `std::iomanip`.

## 2. Path & Asset Resolution

One of the major improvements in this fork is the robust path handling:

1.  **Cross-Platform Path Parsing**: The tool now correctly handles both forward slashes (`/`) and backward slashes (`\`) in command-line arguments.
2.  **Context-Aware Working Directory**: Upon execution, the tool automatically switches its internal context to the directory of the input `.pmx` file. This ensures that:
    *   Relative texture paths (e.g., `tex/skin.png`) are resolved correctly.
    *   External assets are found regardless of whether the tool is called with a relative or absolute path.
3.  **Non-ASCII Sanitization (Latin Conversion)**: 
    *    Filenames containing Japanese/Chinese characters are hex-encoded (e.g., `体.png` $\rightarrow$ `e4bd93.png`) to ensure compatibility with standard FBX importers.
    *   **Automatic Migration**: The tool automatically creates copies of texture files with these sanitized names if necessary.

## 3. Usage Details

### Command Syntax
```cmd
Pmx2Fbx.exe <input_pmx_file_path>
```

### Examples
*   **Relative Path**: `./build/Pmx2Fbx.exe test/model.pmx`
*   **Absolute Path**: `./build/Pmx2Fbx.exe "C:\Users\tomo\Models\model.pmx"`

### Behavior & Output
*   **Output Location**: The tool generates a new file with a **`.pmx.fbx`** extension in the same directory as the source.
    *   *Input*: `model.pmx`
    *   *Output*: `model.pmx.fbx`
*   **FBX Format**: Generates **FBX 7.7.0 Binary** files (compatible with 2019/2020+ modeling tools).
*   **Standalone Binary**: The executable is a single-file portable binary with no external runtime dependencies.

## 4. Build System

The project now uses a CMake-based system with presets for different environments:

*   **Presets**:
    *   `ninja`: Fast CLI build (Default).
    *   `vs2022`: Visual Studio 2022 solution.
    *   `vs2026-slnx`: Modern VS 2026 XML-based solution format.
*   **Target**: 64-bit (x64) only.
*   **Runtime**: Static Runtime Library (/MT).

## 5. AI-Assisted Conversion (Skill)

This project includes a skill for AI assistants to convert MMD models automatically. The skill is installed locally in `.agents/skills/mmd-to-fbx/`.

### Installation

The skill is already included in this repository. To make it fully self-contained, copy `Pmx2Fbx.exe` into the skill's local binary directory:

```
.agents/skills/mmd-to-fbx/bin/Pmx2Fbx.exe
```

> **Note**: The `bin/` directory is excluded from version control via `.gitignore`. You must manually populate it after cloning, either by building from source (`cmake --preset ninja && cmake --build --preset ninja`) or downloading a release binary.

### Requirements

- **Pmx2Fbx.exe** placed in `.agents/skills/mmd-to-fbx/bin/`
- For PowerShell invocation: **PowerShell 5.1+** / **PowerShell 7+**
- For Python invocation: **Python 3.8+** (no additional packages required)

### Usage with AI

When working with an AI assistant, you can request conversions using simple prompts:

```
# Convert all PMX models found recursively in a folder
"Convert all MMD models in C:\Users\tomo\Desktop\MmdRepo to FBX"

# Convert a single PMX file directly
"Convert C:\Models\Miku\Miku.pmx to FBX"
```

### How It Works

1. **Input**: Accepts either a directory path (recursive scan) or a direct `.pmx` file path.
2. **Discovery**: Recursively finds all `.pmx` files under the given directory.
3. **Conversion**: Invokes the locally bundled `bin/Pmx2Fbx.exe` on each discovered model.
4. **Output**: Creates an in-place `.pmx.fbx` file alongside the source, preserving the original textures and materials.

### Skill Files

*   `.agents/skills/mmd-to-fbx/SKILL.md` - Skill documentation and usage instructions
*   `.agents/skills/mmd-to-fbx/bin/Pmx2Fbx.exe` - Locally bundled executable (not tracked in git)
*   `.agents/skills/mmd-to-fbx/scripts/Convert-MmdToFbx.ps1` - PowerShell invocation script
*   `.agents/skills/mmd-to-fbx/scripts/convert_mmd_to_fbx.py` - Python invocation script
