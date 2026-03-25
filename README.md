# Pmx2Fbx Modernized Fork

This is a high-performance, modernized fork of the original Pmx2Fbx tool, designed to convert MikuMikuDance (MMD) `.pmx` model files into Autodesk `.FBX` format. 

## Key Modernizations
*   **Modern C++20 Core**: Completely refactored the codebase to C++20 standards.
*   **RAII Architecture**: Implemented strict RAII (Resource Acquisition Is Initialization) patterns.
    *   **`FbxHelper::Shape`**: Automated lifetime management using `std::unique_ptr`.
    *   **`PmxReader::PmxMorph`**: Replaced raw pointer management with `std::vector<std::unique_ptr<PmxBaseMorph>>`, ensuring zero memory leaks.
*   **Robust Path Handling**: Re-engineered the CLI path parser to handle forward slashes, backslashes, and complex relative paths. The tool now correctly resolves textures and assets regardless of the execution context.
*   **Standalone Portability**: 
    *   Exclusively targets **x64** architecture.
    *   Linked against the **Static C++ Runtime (/MT)** to produce a single, portable `.exe` with no external DLL dependencies (except the FBX SDK).
*   **Modern Build System**: 
    *   Full **CMake** integration (3.15+).
    *   Support for **Ninja** for ultra-fast builds.
    *   Support for the new Visual Studio 2026 **`.slnx`** solution format.
*   **Clean Code Standards**:
    *   Removed legacy `vld.h` dependencies.
    *   Replaced C-style file I/O and memory management (`FILE*`, `malloc`, `free`) with modern alternatives (`std::ifstream`, `std::vector`).
    *   Standardized string handling using `std::to_string`, `std::stringstream`, and `std::iomanip`.

## Prerequisites
*   **CMake** (3.15+)
*   **Autodesk FBX SDK 2020.3.9** (Default path: `C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.9`)
*   **Ninja** (Recommended) or **Visual Studio 2022/2026**

## Building the Project
We provide CMake Presets for a streamlined experience:

```cmd
# Build standalone binary using Ninja
cmake --preset ninja
cmake --build --preset ninja

# Generate a modern Visual Studio 2026 (.slnx) solution
cmake --preset vs2026-slnx
```

The resulting binary will be located at `build/Pmx2Fbx.exe`.

## Quick Start
```cmd
./build/Pmx2Fbx.exe path/to/your/model.pmx
```
The converted file will be saved as **`model.pmx.fbx`** in the same directory as the source.
