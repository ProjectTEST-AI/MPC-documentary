#include <array>
#include <cctype>
#include <ranges>
#include <algorithm>
#include "../include/utils.h"
#include "../include/operations.h"

namespace {
    std::array<char, 256> uppercaseTable = []() {
        std::array<char, 256> table{};
        for (int i = 0; i < 256; ++i) {
            table[i] = static_cast<char>(std::toupper(i));
        }
        return table;
        }();

    [[nodiscard]] std::string toUpper(std::string_view sv) {
        std::string upper(sv);
        std::ranges::transform(upper, upper.begin(),
                               [](unsigned char c) { return uppercaseTable[c]; });
        return upper;
    }

    // Chapter 7: Format F_RR formatter (opcodes 1-51)
    std::string formatRR(std::span<const std::string_view> operands) {
        int opcode = Operations::instructionMap.at(toUpper(operands[0])).opcode;
        uint8_t out = static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[1]))));
        uint8_t src2 = operands.size() > 2 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[2])))) : 0;
        uint64_t encoded = formatF_RR(opcode, out, out, src2);
        return std::to_string(encoded);
    }

    // Chapter 7: Format F_RR unary formatter (single operand)
    std::string formatRRUnary(std::span<const std::string_view> operands) {
        int opcode = Operations::instructionMap.at(toUpper(operands[0])).opcode;
        uint8_t out = static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[1]))));
        uint64_t encoded = formatF_RR(opcode, out, out, 0);
        return std::to_string(encoded);
    }

    // Chapter 7: Format F_RR move/swap formatter (register transfer)
    std::string formatRRTransfer(std::span<const std::string_view> operands) {
        int opcode = Operations::instructionMap.at(toUpper(operands[0])).opcode;
        uint8_t reg1 = static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[1]))));
        uint8_t reg2 = operands.size() > 2 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[2])))) : 0;
        uint64_t encoded = formatF_RR(opcode, reg1, reg2, 0);
        return std::to_string(encoded);
    }

    // Chapter 7: Format F_RI formatter (opcodes 52-61)
    std::string formatRI(std::span<const std::string_view> operands) {
        int opcode = Operations::instructionMap.at(toUpper(operands[0])).opcode;
        uint8_t out = static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[1]))));
        uint32_t imm = operands.size() > 2 ? 
            static_cast<uint32_t>(std::stoul(std::string(stripSymbols(operands[2])))) : 0;
        uint64_t encoded = formatF_RI(opcode, out, imm);
        return std::to_string(encoded);
    }

    // Chapter 7: Format F_RC formatter (opcodes 62-66) - Register-Cache
    std::string formatRC(std::span<const std::string_view> operands) {
        int opcode = Operations::instructionMap.at(toUpper(operands[0])).opcode;
        uint8_t dest = static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[1]))));
        uint8_t addrReg = operands.size() > 2 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[2])))) : 0;
        uint8_t srcReg = operands.size() > 3 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[3])))) : 0;
        uint64_t encoded = formatF_RM(opcode, dest, addrReg, srcReg);
        return std::to_string(encoded);
    }

    // Chapter 7: Format F_CM formatter (opcodes 67-73) - Cache-Memory
    std::string formatCM(std::span<const std::string_view> operands) {
        int opcode = Operations::instructionMap.at(toUpper(operands[0])).opcode;
        uint8_t cacheReg = static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[1]))));
        uint8_t ramReg = operands.size() > 2 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[2])))) : 0;
        uint8_t offsetReg = operands.size() > 3 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[3])))) : 0;
        uint64_t encoded = formatF_RM(opcode, cacheReg, ramReg, offsetReg);
        return std::to_string(encoded);
    }

    // Chapter 7: Format F_RW formatter (opcodes 74-93)
    std::string formatRW(std::span<const std::string_view> operands) {
        int opcode = Operations::instructionMap.at(toUpper(operands[0])).opcode;
        uint8_t out = static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[1]))));
        uint8_t bldg = operands.size() > 2 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[2])))) : 0;
        uint8_t x = operands.size() > 3 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[3])))) : 0;
        uint8_t y = operands.size() > 4 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[4])))) : 0;
        uint64_t encoded = formatF_RW(opcode, out, bldg, x, y);
        return std::to_string(encoded);
    }

    // Chapter 7: Format F_JC formatter (opcodes 94-103)
    std::string formatJC(std::span<const std::string_view> operands) {
        int opcode = Operations::instructionMap.at(toUpper(operands[0])).opcode;
        uint8_t cond = operands.size() > 1 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[1])))) : 0;
        uint8_t reg = operands.size() > 2 ? 
            static_cast<uint8_t>(std::stoul(std::string(stripSymbols(operands[2])))) : 0;
        uint64_t target = operands.size() > 3 ? 
            static_cast<uint64_t>(std::stoull(std::string(stripSymbols(operands[3])))) : 0;
        uint64_t encoded = formatF_JC(opcode, cond, reg, target);
        return std::to_string(encoded);
    }
}

OperandType getOperandType(std::string_view operand) noexcept {
    using enum OperandType;
    if (!operand.empty()) {
        switch (operand[0]) {
        case '#': return Register;  // CPU register (0-63)
        case '@': return Cache;     // L2 Cache address
        case '$': return RAM;       // External RAM module
        default: return Immediate;  // Literal numeric value
        }
    }
    return None;
}

bool isValidInstruction(std::string_view instruction) {
    return Operations::instructionMap.contains(toUpper(instruction));
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
    // ==================== F_RR: Register Transfer (opcodes 1-2) ====================
    {"MOV", {1, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register}, formatRRTransfer}},
    {"SWP", {2, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register}, formatRRTransfer}},

    // ==================== F_RR: Math operations (opcodes 3-29) ====================
    {"ADD", {3, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"SUB", {4, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"MUL", {5, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"DIV", {6, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"IDV", {7, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"MOD", {8, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"TMD", {9, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"POW", {10, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"LOG", {11, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"SQR", {12, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"MAX", {13, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"MIN", {14, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"FLR", {15, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"CIL", {16, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"ABS", {17, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"SIN", {18, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"COS", {19, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"TAN", {20, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"ASN", {21, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"ACS", {22, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"ATN", {23, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"CSC", {24, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"SEC", {25, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"COT", {26, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"ACSC", {27, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"ASEC", {28, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"ACOT", {29, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},

    // ==================== F_RR: Comparisons (opcodes 30-43) ====================
    {"AND", {30, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"OR", {31, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"XOR", {32, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"NOT", {33, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"SHL", {34, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"SHR", {35, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"EQ", {36, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"NEQ", {37, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"STE", {38, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"LAD", {39, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"LTH", {40, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"LTE", {41, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"MTH", {42, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},
    {"MTE", {43, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register | OperandType::Immediate}, formatRR}},

    // ==================== F_RR: Native Mindustry (opcodes 44-51) ====================
    {"A2V", {44, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register}, formatRR}},
    {"ADV", {45, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register}, formatRR}},
    {"LOV", {46, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register}, formatRR}},
    {"2DS", {47, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register}, formatRR}},
    {"RDN", {48, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"PKC", {49, InstructionFormat::F_RR, {OperandType::Register, OperandType::Register, OperandType::Register, OperandType::Register}, formatRR}},
    {"UPC", {50, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},
    {"SGN", {51, InstructionFormat::F_RR, {OperandType::Register}, formatRRUnary}},

    // ==================== F_RI: Immediate operations (opcodes 52-61) ====================
    {"STR", {52, InstructionFormat::F_RI, {OperandType::Register, OperandType::Immediate}, formatRI}},
    {"STRF", {53, InstructionFormat::F_RI, {OperandType::Immediate}, formatRI}},
    {"ADDI", {54, InstructionFormat::F_RI, {OperandType::Register, OperandType::Immediate}, formatRI}},
    {"SUBI", {55, InstructionFormat::F_RI, {OperandType::Register, OperandType::Immediate}, formatRI}},
    {"MULI", {56, InstructionFormat::F_RI, {OperandType::Register, OperandType::Immediate}, formatRI}},
    {"DIVI", {57, InstructionFormat::F_RI, {OperandType::Register, OperandType::Immediate}, formatRI}},
    {"ANDI", {58, InstructionFormat::F_RI, {OperandType::Register, OperandType::Immediate}, formatRI}},
    {"ORI", {59, InstructionFormat::F_RI, {OperandType::Register, OperandType::Immediate}, formatRI}},
    {"XORI", {60, InstructionFormat::F_RI, {OperandType::Register, OperandType::Immediate}, formatRI}},
    {"SHLI", {61, InstructionFormat::F_RI, {OperandType::Register, OperandType::Immediate}, formatRI}},

    // ==================== F_RC: Register-Cache (opcodes 62-66) ====================
    {"LRC", {62, InstructionFormat::F_RC, {OperandType::Register, OperandType::Register}, formatRC}},
    {"SRC", {63, InstructionFormat::F_RC, {OperandType::Register, OperandType::Register}, formatRC}},
    {"XCHC", {64, InstructionFormat::F_RC, {OperandType::Register, OperandType::Register}, formatRC}},
    {"CCPY", {65, InstructionFormat::F_RC, {OperandType::Register, OperandType::Register, OperandType::Register}, formatRC}},

    // ==================== F_CM: Cache-Memory (opcodes 67-73) ====================
    {"LCM", {67, InstructionFormat::F_CM, {OperandType::Register, OperandType::Register}, formatCM}},
    {"SCM", {68, InstructionFormat::F_CM, {OperandType::Register, OperandType::Register}, formatCM}},
    {"LCMS", {69, InstructionFormat::F_CM, {OperandType::Register, OperandType::Register, OperandType::Register}, formatCM}},
    {"SCMS", {70, InstructionFormat::F_CM, {OperandType::Register, OperandType::Register, OperandType::Register}, formatCM}},
    {"CMCPY", {71, InstructionFormat::F_CM, {OperandType::Register, OperandType::Register, OperandType::Register}, formatCM}},
    {"CMSTR", {72, InstructionFormat::F_CM, {OperandType::Register, OperandType::Register, OperandType::Register}, formatCM}},
    {"LEA", {73, InstructionFormat::F_CM, {OperandType::Register, OperandType::Register, OperandType::Register}, formatCM}},

    // ==================== F_RW: World operations (opcodes 74-93) ====================
    {"SENS", {74, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register, OperandType::Register}, formatRW}},
    {"CTRL", {75, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register, OperandType::Register}, formatRW}},
    {"RADR", {76, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register, OperandType::Register, OperandType::Register}, formatRW}},
    {"LINK", {77, InstructionFormat::F_RW, {OperandType::Register, OperandType::Immediate}, formatRW}},
    {"LNKC", {78, InstructionFormat::F_RW, {OperandType::Register}, formatRW}},
    {"DRAW", {79, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register, OperandType::Register}, formatRW}},
    {"DRWF", {80, InstructionFormat::F_RW, {OperandType::Register}, formatRW}},
    {"DCOL", {81, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register, OperandType::Register, OperandType::Register}, formatRW}},
    {"DSTR", {82, InstructionFormat::F_RW, {OperandType::Register}, formatRW}},
    {"PRNT", {83, InstructionFormat::F_RW, {OperandType::Register}, formatRW}},
    {"PRTF", {84, InstructionFormat::F_RW, {OperandType::Register}, formatRW}},
    {"UBND", {85, InstructionFormat::F_RW, {OperandType::Register}, formatRW}},
    {"UCTL", {86, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register, OperandType::Register}, formatRW}},
    {"URDR", {87, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register, OperandType::Register}, formatRW}},
    {"ULOC", {88, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register}, formatRW}},
    {"WSET", {89, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register, OperandType::Register}, formatRW}},
    {"WFLG", {90, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register}, formatRW}},
    {"WRUL", {91, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register}, formatRW}},
    {"WFCH", {92, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register, OperandType::Register, OperandType::Register}, formatRW}},
    {"WEFF", {93, InstructionFormat::F_RW, {OperandType::Register, OperandType::Register, OperandType::Register, OperandType::Register}, formatRW}},

    // ==================== F_JC: Flow control (opcodes 94-103) ====================
    {"JMP", {94, InstructionFormat::F_JC, {OperandType::Immediate | OperandType::Cache}, formatJC}},
    {"JEQ", {95, InstructionFormat::F_JC, {OperandType::Register, OperandType::Register, OperandType::Immediate | OperandType::Cache}, formatJC}},
    {"JNE", {96, InstructionFormat::F_JC, {OperandType::Register, OperandType::Register, OperandType::Immediate | OperandType::Cache}, formatJC}},
    {"JLT", {97, InstructionFormat::F_JC, {OperandType::Register, OperandType::Register, OperandType::Immediate | OperandType::Cache}, formatJC}},
    {"JLE", {98, InstructionFormat::F_JC, {OperandType::Register, OperandType::Register, OperandType::Immediate | OperandType::Cache}, formatJC}},
    {"JGT", {99, InstructionFormat::F_JC, {OperandType::Register, OperandType::Register, OperandType::Immediate | OperandType::Cache}, formatJC}},
    {"JGE", {100, InstructionFormat::F_JC, {OperandType::Register, OperandType::Register, OperandType::Immediate | OperandType::Cache}, formatJC}},
    {"CALL", {101, InstructionFormat::F_JC, {OperandType::Immediate | OperandType::Cache}, formatJC}},
    {"RET", {102, InstructionFormat::F_JC, {}, formatJC}},
    {"HLT", {103, InstructionFormat::F_JC, {}, formatJC}}
};
