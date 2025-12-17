#pragma once

#include <string>
#include <string_view>
#include "utils.h"

// Parse a single instruction line and return the encoded result
[[nodiscard]] std::string parseInstruction(std::string_view lineView, size_t lineNumber, JumpLabels& jumpLabels);

// Single-threaded compilation
void compileSingleThread(struct LineSource& source);
