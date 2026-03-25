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
