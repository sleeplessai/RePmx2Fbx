# Project GEMINI: Pmx2Fbx Modernized

## Project Overview
Pmx2Fbx is a specialized C++ tool for converting MikuMikuDance (MMD) `.pmx` model files into the Autodesk `.fbx` format. This fork has been modernized to meet high standards of safety, performance, and portability.

The project is structured into three layers:
- **PmxReader**: A binary parser (`PmxLib/PmxReader.h`) that extracts geometry, skeletal hierarchies, and materials.
- **FbxHelper**: A wrapper for the FBX SDK (`PmxLib/FbxHelper.h/cpp`) that constructs the scene using RAII patterns.
- **Conversion Controller**: The main logic (`PmxLib/main.cpp`) coordinating the translation and path resolution.

### Modernization Features
- **C++20 & RAII**: Uses `std::unique_ptr` for automated resource management and `std::vector` for safe memory handling.
- **Robust Path Resolution**: Correctly handles both forward and backward slashes in input paths and resolves relative assets reliably.
- **Standalone Binary**: Produces a single-file x64 binary linked against the static C++ runtime (/MT).
- **Automated Sanitization**: Converts non-ASCII names to hex strings and automatically migrates texture files to ensure cross-platform compatibility.

## Building and Running

### Build Prerequisites
- **CMake**: 3.15 or higher.
- **Autodesk FBX SDK**: Version 2020.3.9.
- **Compiler**: MSVC 2022 (v143) or 2026 (v145) with C++20 support.

### Build Presets
The project uses `CMakePresets.json`:
1.  **Ninja (CLI)**: `cmake --preset ninja && cmake --build --preset ninja`
2.  **VS 2026 (.slnx)**: `cmake --preset vs2026-slnx`
3.  **VS 2022 (.sln)**: `cmake --preset vs2022`

The executable is located in the `build/` directory for CLI builds.

### Running the Tool
```cmd
./build/Pmx2Fbx.exe <input_path.pmx>
```
The resulting file is saved as `<input_name>.pmx.fbx` in the same directory as the source.

## Architecture and Design
- **Memory Safety**: `FbxHelper::Shape` and `PmxMorph` use smart pointers to ensure zero memory leaks.
- **Dependencies**: Links against static `libfbxsdk-mt.lib`, `libxml2-mt.lib`, `zlib-mt.lib`, `shlwapi.lib`, and `wininet.lib`.
- **Coordinate System**: MMD's DirectX coordinates are converted to FBX format, including flipping UVs on the Y-axis.

## Known Limitations
- [ ] **Morph Export**: Morph data is parsed but not yet exported to FBX.
- [ ] **Physics Support**: Rigid Body and Joint data are parsed but ignored during conversion.
- [ ] **Scale Config**: No CLI flags currently exist for output scaling.
