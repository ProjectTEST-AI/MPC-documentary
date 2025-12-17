#include <cctype>
#include "../include/utils.h"

// Chapter 7: Format F_RR encoding
// [52:46] opcode(7b) [45:40] out(6b) [39:34] src1(6b) [33:28] src2(6b) [27:22] opt1(6b) [21:16] opt2(6b) [15:0] padding(16b)
uint64_t formatF_RR(int opcode, uint8_t out, uint8_t src1, uint8_t src2, uint8_t opt1, uint8_t opt2) {
    uint64_t result = 0;
    result |= (static_cast<uint64_t>(opcode) & 0x7F) << 46;  // 7-bit opcode
    result |= (static_cast<uint64_t>(out) & 0x3F) << 40;     // 6-bit out
    result |= (static_cast<uint64_t>(src1) & 0x3F) << 34;    // 6-bit src1
    result |= (static_cast<uint64_t>(src2) & 0x3F) << 28;    // 6-bit src2
    result |= (static_cast<uint64_t>(opt1) & 0x3F) << 22;    // 6-bit opt1
    result |= (static_cast<uint64_t>(opt2) & 0x3F) << 16;    // 6-bit opt2
    // padding [15:0] is zeros
    return result;
}

// Chapter 7: Format F_RI encoding
// [52:46] opcode(7b) [45:40] out(6b) [39:8] src1(32b) [7:0] padding(8b)
uint64_t formatF_RI(int opcode, uint8_t out, uint32_t immediate) {
    uint64_t result = 0;
    result |= (static_cast<uint64_t>(opcode) & 0x7F) << 46;  // 7-bit opcode
    result |= (static_cast<uint64_t>(out) & 0x3F) << 40;     // 6-bit out
    result |= (static_cast<uint64_t>(immediate) & 0xFFFFFFFF) << 8;  // 32-bit immediate
    // padding [7:0] is zeros
    return result;
}

// Chapter 7: Format F_RM encoding (Register-Indirect)
// [52:46] opcode(7b) [45:40] dest(6b) [39:34] addrReg(6b) [33:28] srcReg(6b) [27:0] padding(28b)
uint64_t formatF_RM(int opcode, uint8_t dest, uint8_t addrReg, uint8_t srcReg) {
    uint64_t result = 0;
    result |= (static_cast<uint64_t>(opcode) & 0x7F) << 46;     // 7-bit opcode
    result |= (static_cast<uint64_t>(dest) & 0x3F) << 40;       // 6-bit dest
    result |= (static_cast<uint64_t>(addrReg) & 0x3F) << 34;    // 6-bit addrReg
    result |= (static_cast<uint64_t>(srcReg) & 0x3F) << 28;     // 6-bit srcReg
    // padding [27:0] is zeros
    return result;
}

// Chapter 7: Format F_RW encoding
// [52:46] opcode(7b) [45:40] out(6b) [39:34] bldg(6b) [33:28] x(6b) [27:22] y(6b) [21:0] padding(22b)
uint64_t formatF_RW(int opcode, uint8_t out, uint8_t bldg, uint8_t x, uint8_t y) {
    uint64_t result = 0;
    result |= (static_cast<uint64_t>(opcode) & 0x7F) << 46;  // 7-bit opcode
    result |= (static_cast<uint64_t>(out) & 0x3F) << 40;     // 6-bit out
    result |= (static_cast<uint64_t>(bldg) & 0x3F) << 34;    // 6-bit bldg
    result |= (static_cast<uint64_t>(x) & 0x3F) << 28;       // 6-bit x
    result |= (static_cast<uint64_t>(y) & 0x3F) << 22;       // 6-bit y
    // padding [21:0] is zeros
    return result;
}

// Chapter 7: Format F_JC encoding
// [52:46] opcode(7b) [45:40] cond(6b) [39:34] reg(6b) [33:0] target(34b)
uint64_t formatF_JC(int opcode, uint8_t cond, uint8_t reg, uint64_t target) {
    uint64_t result = 0;
    result |= (static_cast<uint64_t>(opcode) & 0x7F) << 46;  // 7-bit opcode
    result |= (static_cast<uint64_t>(cond) & 0x3F) << 40;    // 6-bit cond
    result |= (static_cast<uint64_t>(reg) & 0x3F) << 34;     // 6-bit reg
    result |= (target & 0x3FFFFFFFF);                         // 34-bit target
    return result;
}

std::string_view stripSymbols(std::string_view value) noexcept {
    auto it = std::ranges::find_if(value, [](char c) { return std::isdigit(c) != 0; });
    return std::string_view(it, value.end());
}

std::vector<std::string_view> split(std::string_view s, char delimiter) {
    std::vector<std::string_view> tokens;
    tokens.reserve(5); // Reserve space for an estimated number of tokens
    size_t start = 0;
    size_t end = s.find(delimiter);
    while (end != std::string_view::npos) {
        tokens.emplace_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delimiter, start);
    }
    tokens.emplace_back(s.substr(start));
    return tokens;
}


void JumpLabels::addLabel(std::string_view label, int lineNumber) {
    std::lock_guard lock(labelMutex);
    labels[std::string(label)] = lineNumber;
}

int JumpLabels::getLineNumber(std::string_view label) {
    std::lock_guard lock(labelMutex);
    if (auto it = labels.find(std::string(label)) ; it != labels.end()) {
        return it->second;
    }
    return -1; 
}
