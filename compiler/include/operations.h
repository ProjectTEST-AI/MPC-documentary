#pragma once

#include <span>
#include <string>
#include <vector>
#include <functional>
#include <string_view>
#include <unordered_map>


enum class OperandType {
  None = 0,
  Register = 1 << 0,  // CPU register (0-63) - prefix #
  Immediate = 1 << 2, // Literal numeric value - no prefix
  Cache = 1 << 1,     // L2 Cache address - prefix @
  RAM = 1 << 3        // External RAM module - prefix $
};

// Chapter 7: Dynamic instruction formats based on opcode ranges
enum class InstructionFormat {
  F_0,  // NOOP (opcode 0)
  F_RR, // Register-Register (opcodes 1-51)
  F_RI, // Register-Immediate (opcodes 52-61)
  F_RC, // Register-Cache (opcodes 62-66)
  F_CM, // Cache-Memory (opcodes 67-76)
  F_RW, // Register-World (opcodes 77-96)
  F_JC, // Flow Control (opcodes 97-106)
  F_EX  // Exception (opcode 127)
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

// Get instruction format based on opcode
[[nodiscard]] constexpr InstructionFormat getInstructionFormat(int opcode) noexcept {
  if (opcode == 0)
    return InstructionFormat::F_0;
  if (opcode >= 1 && opcode <= 51)
    return InstructionFormat::F_RR;
  if (opcode >= 52 && opcode <= 61)
    return InstructionFormat::F_RI;
  if (opcode >= 62 && opcode <= 66)
    return InstructionFormat::F_RC;
  if (opcode >= 67 && opcode <= 73)
    return InstructionFormat::F_CM;
  if (opcode >= 74 && opcode <= 93)
    return InstructionFormat::F_RW;
  if (opcode >= 94 && opcode <= 103)
    return InstructionFormat::F_JC;
  if (opcode == 127)
    return InstructionFormat::F_EX;
  return InstructionFormat::F_0; // Invalid opcode treated as NOP
}

struct InstructionInfo {
  int opcode;
  InstructionFormat format;
  std::vector<OperandType> allowedOperands;
  std::function<std::string(std::span<const std::string_view>)> formatFunc;
};

class Operations {
public:
  static const std::unordered_map<std::string_view, InstructionInfo> instructionMap;
};

[[nodiscard]] bool isValidInstruction(std::string_view instruction);
[[nodiscard]] bool validateOperands(std::span<const std::string_view> operands,
                                    std::span<const OperandType> allowedTypes);
