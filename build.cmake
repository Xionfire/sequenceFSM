#!/usr/bin/env cmake -P
# ============================================================================
# HighLevelSeq Build Script (CMake Script Mode)
# ============================================================================
# Generic build script that works on Windows, macOS, and Linux
# Usage: cmake -P build.cmake
# ============================================================================

# Set build type
set(BUILD_TYPE "Debug" CACHE STRING "Build type")
set(BUILD_DIR "build")
set(SOURCE_DIR ".")

# Create build directory
file(MAKE_DIRECTORY "${BUILD_DIR}")

# Display information
message(STATUS "============================================================")
message(STATUS "HighLevelSeq Build Script")
message(STATUS "============================================================")
message(STATUS "Build Directory: ${BUILD_DIR}")
message(STATUS "Build Type: ${BUILD_TYPE}")
message(STATUS "Source Directory: ${SOURCE_DIR}")
message(STATUS "")

# Configure
message(STATUS "[*] Configuring CMake...")
execute_process(
    COMMAND ${CMAKE_COMMAND} -G "Visual Studio 18 2026" ..
    WORKING_DIRECTORY "${BUILD_DIR}"
    RESULT_VARIABLE CONFIG_RESULT
)

if(NOT CONFIG_RESULT EQUAL 0)
    message(FATAL_ERROR "[!] CMake configuration failed")
endif()

message(STATUS "[+] Configuration successful")
message(STATUS "")

# Build
message(STATUS "[*] Building project...")
execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --config ${BUILD_TYPE}
    WORKING_DIRECTORY "${BUILD_DIR}"
    RESULT_VARIABLE BUILD_RESULT
)

if(NOT BUILD_RESULT EQUAL 0)
    message(FATAL_ERROR "[!] Build failed")
endif()

message(STATUS "[+] Build successful")
message(STATUS "")

# List executables
message(STATUS "[*] Executables created:")
message(STATUS "   - ${BUILD_DIR}/${BUILD_TYPE}/foo_creation_test.exe")
message(STATUS "   - ${BUILD_DIR}/${BUILD_TYPE}/sequence_exporter_test.exe")
message(STATUS "   - ${BUILD_DIR}/${BUILD_TYPE}/sequence_main.exe")
message(STATUS "")

message(STATUS "============================================================")
message(STATUS "Build completed successfully!")
message(STATUS "============================================================")
