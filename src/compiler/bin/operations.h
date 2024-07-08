#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <vector>
#include "utils.h"

struct InstructionInfo {
    int opcode;
    int operandCount;
    std::function<std::string(std::span<const std::string_view>)> formatFunc;
};

class Operations {
public:
    static const std::unordered_map<std::string_view, InstructionInfo> instructionMap;
};

#endif
