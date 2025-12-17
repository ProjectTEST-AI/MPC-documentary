cmake_minimum_required(VERSION 3.20)
project(MachC_Compiler VERSION 1.0 LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Source files
set(SOURCES
    src/main.cpp
    src/compiler.cpp
    src/decompiler.cpp
    src/multithreading.cpp
    src/operations.cpp
    src/utils.cpp
    src/logging.cpp
)

# Header files (for IDE integration)
set(HEADERS
    include/utils.h
    include/operations.h
    include/compiler.h
    include/decompiler.h
    include/multithreading.h
    include/logging.h
    include/exceptions.h
    include/memory_mapped_file.hpp
)

# Create executable
add_executable(machc ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(machc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Platform-specific settings
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(machc PRIVATE pthread)
elseif(WIN32)
    # Windows-specific settings if needed
endif()

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(machc PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
    )
elseif(MSVC)
    target_compile_options(machc PRIVATE
        /W4
        /permissive-
    )
endif()

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(machc PRIVATE DEBUG=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(machc PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang>:-O3>)
endif()

# Installation
install(TARGETS machc DESTINATION bin)
