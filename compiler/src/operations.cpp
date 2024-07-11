#include "../include/operations.h"
#include "../include/logging.h"
#include "../include/utils.h"
#include <format>
#include <array>
#include <algorithm>
#include <cctype>

namespace {
    std::string formatBinary(std::span<const std::string_view> operands) {
        log(LogLevel::LOW, "Formatting binary operation");
        int opcode = Operations::instructionMap.at(operands[0]).opcode;
        std::array<int, 2> dataTypes = { getDataType(operands[1]), getDataType(operands[2]) };
        std::array<bool, 2> negatives = { isNegative(operands[1]), isNegative(operands[2]) };
        std::array<uint32_t, 2> values = {
            static_cast<uint32_t>(std::stoul(std::string(stripSymbols(operands[1])))),
            static_cast<uint32_t>(std::stoul(std::string(stripSymbols(operands[2]))))
        };
        uint64_t encodedInstruction = formatInstructionToInt(opcode, dataTypes[0], dataTypes[1], negatives[0], negatives[1], values[0], values[1]);
        return std::to_string(encodedInstruction);
    }

    std::string formatUnary(std::span<const std::string_view> operands) {
        log(LogLevel::LOW, "Formatting unary operation");
        int opcode = Operations::instructionMap.at(operands[0]).opcode;
        int dataType = getDataType(operands[1]);
        bool negative = isNegative(operands[1]);
        uint32_t value = static_cast<uint32_t>(std::stoul(std::string(stripSymbols(operands[1]))));
        uint64_t encodedInstruction = formatInstructionToInt(opcode, dataType, 0, negative, false, value, 0);
        return std::to_string(encodedInstruction);
    }
}

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

const std::unordered_map<std::string_view, InstructionInfo> Operations::instructionMap = {
    {"ADD", {1, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"SUB", {2, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"MOV", {3, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"INC", {4, {OperandType::Register | OperandType::Memory}, formatUnary}},
    {"DEC", {5, {OperandType::Register | OperandType::Memory}, formatUnary}},
    {"NEG", {6, {OperandType::Register | OperandType::Memory}, formatUnary}}
    //TODO: ADD MORE SHIT HERE!!! DID YOU JUST FORGOT?
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
