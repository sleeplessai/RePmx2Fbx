cmake_minimum_required(VERSION 3.15)
project(Pmx2Fbx)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define FBX SDK path
set(FBX_SDK_ROOT "C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.9")

# Enforce x64 architecture
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "This project must be built for x64 architecture.")
endif()

# Set paths
set(FBX_INCLUDE_DIR "${FBX_SDK_ROOT}/include")
set(FBX_LIB_DIR "${FBX_SDK_ROOT}/lib/x64")

# Configuration-specific lib path
if(MSVC)
    add_compile_definitions(_XM_NO_INTRINSICS_)
    # Set static runtime (/MT)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/PmxLib
    ${FBX_INCLUDE_DIR}
)

# Source files
set(SOURCES
    PmxLib/Main.cpp
    PmxLib/FbxHelper.cpp
    PmxLib/Utils.cpp
)

set(HEADERS
    PmxLib/BinReader.h
    PmxLib/FbxHelper.h
    PmxLib/MathVector.h
    PmxLib/PmxReader.h
    PmxLib/Utils.h
)

# Executable
add_executable(Pmx2Fbx ${SOURCES} ${HEADERS})

# Link FBX SDK
if(MSVC)
    target_link_directories(Pmx2Fbx PRIVATE
        "$<$<CONFIG:Debug>:${FBX_LIB_DIR}/debug>"
        "$<$<CONFIG:Release>:${FBX_LIB_DIR}/release>"
    )
    
    # Use -mt versions for static linking
    target_link_libraries(Pmx2Fbx PRIVATE 
        $<$<CONFIG:Debug>:libfbxsdk-mt.lib>
        $<$<CONFIG:Debug>:libxml2-mt.lib>
        $<$<CONFIG:Debug>:zlib-mt.lib>
        $<$<CONFIG:Release>:libfbxsdk-mt.lib>
        $<$<CONFIG:Release>:libxml2-mt.lib>
        $<$<CONFIG:Release>:zlib-mt.lib>
        shlwapi.lib
        wininet.lib
    )
endif()

# Output binary directly to the build root, bypassing configuration subfolders
set_target_properties(Pmx2Fbx PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}"
)
