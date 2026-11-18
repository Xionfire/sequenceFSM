cmake_minimum_required(VERSION 3.10)
project(HighLevelSeq)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib)

# Source files
set(LIB_SOURCES
    lib/foo.cpp
    lib/device.cpp
    lib/functions.cpp
)

# Create library
add_library(hightlevelseq ${LIB_SOURCES})

# Foo Creation Example Executable
add_executable(foo_creation_test app/example_foo_creation.cpp)
target_link_libraries(foo_creation_test hightlevelseq)

# Sequence Exporter Example Executable
add_executable(sequence_exporter_test app/example_sequence_exporter.cpp)
target_link_libraries(sequence_exporter_test hightlevelseq)

# Sequence Main Example Executable
add_executable(sequence_main app/sequence_main.cpp)
target_link_libraries(sequence_main hightlevelseq)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
