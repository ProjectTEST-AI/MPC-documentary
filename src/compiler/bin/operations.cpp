#include <format>
#include <array>
#include "operations.h"
#include "logging.h"
#include "utils.h"

namespace {
    std::string formatBinary(std::span<const std::string_view> operands) {
        log(LogLevel::LOW, "Formatting binary operation");
        int opcode = Operations::instructionMap.at(operands[0]).opcode;
        std::array<int, 2> dataTypes = { getDataType(operands[1]), getDataType(operands[2]) };
        std::array<int, 2> negatives = { isNegative(operands[1]), isNegative(operands[2]) };
        std::array<std::string_view, 2> rawValues = { stripSymbols(operands[1]), stripSymbols(operands[2]) };
        return formatInstruction(opcode, dataTypes, negatives, rawValues);
    }

    std::string formatUnary(std::span<const std::string_view> operands) {
        log(LogLevel::LOW, "Formatting unary operation");
        int opcode = Operations::instructionMap.at(operands[0]).opcode;
        int dataType = getDataType(operands[1]);
        int negative = isNegative(operands[1]);
        std::string_view rawValue = stripSymbols(operands[1]);
        return std::format("{} {} {} {}", opcode, dataType, negative, rawValue);
    }
}

const std::unordered_map<std::string_view, InstructionInfo> Operations::instructionMap = {
    {"ADD", {1, 2, formatBinary}},
    {"SUB", {2, 2, formatBinary}}
    //! Format for new instructions {"instruction shorthand name", {(instruction opCode, (operand count), (formatting style)}}
    // TODO: Add more operations here
};