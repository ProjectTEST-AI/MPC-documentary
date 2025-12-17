#pragma once

#include <string>
#include <cstdint>
#include "multithreading.h"

// Decompile functions - decode encoded instruction back to assembly
[[nodiscard]] std::string decompileInstruction(uint64_t encoded);

// Single-threaded decompilation
void decompileSingleThread(LineSource& source);

