#include "../include/operations.h"
#include "../include/logging.h"
#include "../include/utils.h"
#include <format>
#include <array>
#include <algorithm>
#include <cctype>

namespace {
    std::array<char, 256> uppercaseTable = []() {
        std::array<char, 256> table{};
        for (int i = 0; i < 256; ++i) {
            table[i] = static_cast<char>(std::toupper(i));
        }
        return table;
        }();

    [[nodiscard]] std::string_view toUpperView(std::string_view sv) {
        const char* data = sv.data();
        for (size_t i = 0; i < sv.size(); ++i) {
            if (data[i] != uppercaseTable[static_cast<unsigned char>(data[i])]) {
                std::string upper(sv);
                std::transform(upper.begin() + i, upper.end(), upper.begin() + i,
                    [](unsigned char c) { return uppercaseTable[c]; });
                return upper;
            }
        }
        return sv;
    }

    std::string formatBinary(std::span<const std::string_view> operands) {
        int opcode = Operations::instructionMap.at(toUpperView(operands[0])).opcode;
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
        int opcode = Operations::instructionMap.at(toUpperView(operands[0])).opcode;
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

bool isValidInstruction(std::string_view instruction) {
    return Operations::instructionMap.find(toUpperView(instruction)) != Operations::instructionMap.end();
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

const std::unordered_map<std::string_view, InstructionInfo> Operations::instructionMap = {
    // Math operations
    {"ADD", {1, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"SUB", {2, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"MUL", {3, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"DIV", {4, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"IDIV", {5, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"MOD", {6, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"POW", {7, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"EXP", {8, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"LOG", {9, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"LN", {10, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"SQRT", {11, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"MAX", {12, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"MIN", {13, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"FLR", {14, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"CEIL", {15, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"ABS", {16, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"SIN", {17, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"COS", {18, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"TAN", {19, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"ASIN", {20, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"ACOS", {21, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"ATAN", {22, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"AOV", {23, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"DOV", {24, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"LOV", {25, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"SXN", {26, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"RND", {27, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"CSC", {28, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"SEC", {29, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"COT", {30, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"ACSC", {31, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"ASEC", {32, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},
    {"ACOT", {33, {OperandType::Register | OperandType::Memory | OperandType::Negative}, formatUnary}},

    // Data comparisons
    {"EQL", {34, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"NEQ", {35, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"LAND", {36, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"LTH", {37, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"LET", {38, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"MTH", {39, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"MET", {40, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"SEQ", {41, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"BOR", {42, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"BAND", {43, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"BXOR", {44, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"BFL", {45, {OperandType::Register | OperandType::Memory}, formatUnary}},
    {"BSL", {46, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},
    {"BSR", {47, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate}, formatBinary}},

    //TODO: Flow control
    // soonTM
    //! jump is currently hardcoded

    // Data management
    // {"SAV", {52, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    // {"LOD", {53, {OperandType::Register | OperandType::Memory | OperandType::Negative, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    {"MOV", {54, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory}, formatBinary}},
    {"SET", {55, {OperandType::Register | OperandType::Memory, OperandType::Register | OperandType::Memory | OperandType::Immediate | OperandType::Negative}, formatBinary}},
    // {"LSUB", {56, {, }, }},
    // {"USUB", {56, {, }, }},
    // {"DHLP", {57, {, }, }},
    // {"DREG", {58, {, }, }},

    //TODO: ADD MORE HERE!!!
};
