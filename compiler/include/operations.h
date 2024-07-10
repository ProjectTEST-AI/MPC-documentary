#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <vector>
#include <span>

enum class OperandType {
    None = 0,
    Register = 1 << 0,
    Memory = 1 << 1,
    Immediate = 1 << 2,
    Negative = 1 << 3
};

constexpr OperandType operator|(OperandType a, OperandType b) {
    return static_cast<OperandType>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr OperandType operator&(OperandType a, OperandType b) {
    return static_cast<OperandType>(static_cast<int>(a) & static_cast<int>(b));
}

[[nodiscard]] constexpr bool isOperandTypeValid(OperandType allowedTypes, OperandType actualType) {
    return (allowedTypes & actualType) == actualType;
}

[[nodiscard]] OperandType getOperandType(std::string_view operand) noexcept;

struct InstructionInfo {
    int opcode;
    std::vector<OperandType> allowedOperands;
    std::function<std::string(std::span<const std::string_view>)> formatFunc;
};

class Operations {
public:
    static const std::unordered_map<std::string_view, InstructionInfo> instructionMap;
};

[[nodiscard]] bool isValidInstruction(std::string_view instruction);
[[nodiscard]] bool validateOperands(std::span<const std::string_view> operands, std::span<const OperandType> allowedTypes);

#endif
