#include "../include/operations.h"
#include "../include/logging.h"
#include "../include/utils.h"
#include <format>
#include <array>
#include <algorithm>
#include <cctype>

OperandType getOperandType(std::string_view operand) noexcept {
    OperandType type = OperandType::None;

    if (!operand.empty() && operand[0] == '-') {
        type = type | OperandType::Negative;
        operand.remove_prefix(1);
    }

    if (!operand.empty()) {
        switch (operand[0]) {
        case '#': return type | OperandType::Register;
        case '@': return type | OperandType::Memory;
        default: return type | OperandType::Immediate;
        }
    }
    return type;
}

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
    {"ADD", {1, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"SUB", {2, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"MOV", {3, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"INC", {4, {OperandType::Register | OperandType::Memory}, formatUnary}},
    {"DEC", {5, {OperandType::Register | OperandType::Memory}, formatUnary}},
    {"NEG", {6, {OperandType::Register | OperandType::Memory}, formatUnary}},
};

bool isValidInstruction(std::string_view instruction) {
    return Operations::instructionMap.find(instruction) != Operations::instructionMap.end();
}

bool validateOperands(std::span<const std::string_view> operands, std::span<const OperandType> allowedTypes) {
    if (operands.size() != allowedTypes.size()) return false;

    for (size_t i = 0; i < operands.size(); ++i) {
        OperandType type = getOperandType(operands[i]);
        if (!isOperandTypeValid(allowedTypes[i], type)) {
            return false;
        }
    }
    return true;
}
