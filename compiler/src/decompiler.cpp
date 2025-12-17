#include <array>
#include <format>
#include <string_view>
#include <unordered_map>
#include "../include/logging.h"
#include "../include/decompiler.h"
#include "../include/operations.h"
#include "../include/multithreading.h"

namespace {
    struct OpcodeEntry {
        std::string_view mnemonic;
        const InstructionInfo* info;
    };

    std::unordered_map<int, OpcodeEntry> buildOpcodeMap() {
        std::unordered_map<int, OpcodeEntry> map;
        for (const auto& [mnemonic, info] : Operations::instructionMap) {
            map[info.opcode] = {mnemonic, &info};
        }
        return map;
    }

    const auto opcodeMap = buildOpcodeMap();

    // Lookup helper that returns both mnemonic and info in one call
    const OpcodeEntry* lookupOpcode(int opcode) {
        auto it = opcodeMap.find(opcode);
        return (it != opcodeMap.end()) ? &it->second : nullptr;
    }

    // Format operands based on extracted values and operand count
    template<typename... Args>
    std::string formatOperands(size_t count, Args... args) {
        std::array<uint64_t, sizeof...(Args)> values = {static_cast<uint64_t>(args)...};
        std::string result;
        for (size_t i = 0; i < count && i < values.size(); ++i) {
            result += std::format(" #{}", values[i]);
        }
        return result;
    }
}

std::string_view getInstructionMnemonic(int opcode) {
    const auto* entry = lookupOpcode(opcode);
    return entry ? entry->mnemonic : "???";
}

std::string decompileInstruction(uint64_t encoded) {
    // Extract opcode from bits [52:46] (7 bits)
    int opcode = static_cast<int>((encoded >> 46) & 0x7F);
    
    if (opcode == 0) {
        return "NOOP";
    }
    
    const auto* entry = lookupOpcode(opcode);
    if (!entry) {
        return std::format("; Unknown opcode {}", opcode);
    }
    
    const auto& info = *entry->info;
    const size_t operandCount = info.allowedOperands.size();
    std::string result(entry->mnemonic);
    
    switch (info.format) {
        case InstructionFormat::F_0:
            break;
            
        case InstructionFormat::F_RR: {
            uint8_t out = static_cast<uint8_t>((encoded >> 40) & 0x3F);
            uint8_t src1 = static_cast<uint8_t>((encoded >> 34) & 0x3F);
            uint8_t src2 = static_cast<uint8_t>((encoded >> 28) & 0x3F);
            
            if (operandCount == 1) {
                result += std::format(" #{}", out);
            } else if (operandCount == 2) {
                // Transfer instructions (MOV, SWP) use src1, others use src2
                result += std::format(" #{} #{}", out, (opcode <= 2) ? src1 : src2);
            } else if (operandCount >= 3) {
                result += std::format(" #{} #{} #{}", out, src1, src2);
            }
            break;
        }
        
        case InstructionFormat::F_RI: {
            uint8_t out = static_cast<uint8_t>((encoded >> 40) & 0x3F);
            uint32_t imm = static_cast<uint32_t>((encoded >> 8) & 0xFFFFFFFF);
            result += std::format(" #{} {}", out, imm);
            break;
        }
        
        case InstructionFormat::F_RC:
        case InstructionFormat::F_CM: {
            uint8_t dest = static_cast<uint8_t>((encoded >> 40) & 0x3F);
            uint8_t addrReg = static_cast<uint8_t>((encoded >> 34) & 0x3F);
            uint8_t srcReg = static_cast<uint8_t>((encoded >> 28) & 0x3F);
            
            if (operandCount == 2) {
                result += std::format(" #{} #{}", dest, addrReg);
            } else {
                result += std::format(" #{} #{} #{}", dest, addrReg, srcReg);
            }
            break;
        }
        
        case InstructionFormat::F_RW: {
            uint8_t out = static_cast<uint8_t>((encoded >> 40) & 0x3F);
            uint8_t bldg = static_cast<uint8_t>((encoded >> 34) & 0x3F);
            uint8_t x = static_cast<uint8_t>((encoded >> 28) & 0x3F);
            uint8_t y = static_cast<uint8_t>((encoded >> 22) & 0x3F);
            
            const uint8_t values[] = {out, bldg, x, y};
            for (size_t i = 0; i < operandCount && i < 4; ++i) {
                result += std::format(" #{}", values[i]);
            }
            break;
        }
        
        case InstructionFormat::F_JC: {
            uint8_t cond = static_cast<uint8_t>((encoded >> 40) & 0x3F);
            uint8_t reg = static_cast<uint8_t>((encoded >> 34) & 0x3F);
            uint64_t target = encoded & 0x3FFFFFFFF;
            
            if (operandCount == 1) {
                result += std::format(" {}", target);
            } else if (operandCount >= 3) {
                result += std::format(" #{} #{} {}", cond, reg, target);
            }
            // operandCount == 0: RET, HLT - no operands
            break;
        }
        
        case InstructionFormat::F_EX:
            result = "EXCEPTION";
            break;
    }
    
    return result;
}

void decompileSingleThread(LineSource& source) {
    size_t lineNumber = 1;
    while (auto line = source.next()) {
        std::string_view trimmedLine = trim(std::string_view(*line));
        if (trimmedLine.empty()) {
            continue;
        }
        try {
            uint64_t encoded = std::stoull(std::string(trimmedLine));
            std::string result = decompileInstruction(encoded) + '\n';
            bufferOutput(std::move(result));
        } catch (const std::exception&) {
            log.warn(std::format("Invalid encoded value '{}' on line {}, skipping", trimmedLine, lineNumber));
        }
        lineNumber++;
    }
}
