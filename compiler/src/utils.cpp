#include "../include/utils.h"
#include <cctype>
#include <format>
#include <bitset>
#include <limits>

uint64_t formatInstructionToInt(int opcode, int dataType1, int dataType2, bool negative1, bool negative2, uint32_t value1, uint32_t value2) {
    uint64_t result = 0;

    // Opcode (8 bits)
    result |= (static_cast<uint64_t>(opcode) & 0xFF) << 56;

    // Data types (2 bits each)
    result |= (static_cast<uint64_t>(dataType1) & 0x3) << 54;
    result |= (static_cast<uint64_t>(dataType2) & 0x3) << 52;

    // Negative flags (1 bit each)
    result |= (negative1 ? 1ULL : 0ULL) << 51;
    result |= (negative2 ? 1ULL : 0ULL) << 50;

    // Values (20 bits each)
    result |= (static_cast<uint64_t>(value1) & 0xFFFFF) << 20;
    result |= (static_cast<uint64_t>(value2) & 0xFFFFF);

    return result;
}

/* old formatting
std::string formatInstruction(int opcode, std::span<const int> dataTypes, std::span<const int> negatives, std::span<const std::string_view> rawValues) {
   return std::format("{} {} {} {} {} {} {}", opcode, dataTypes[0], dataTypes[1], negatives[0], negatives[1], rawValues[0], rawValues[1]);
}
*/

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
    labels.emplace(std::string(label), lineNumber);
}

int JumpLabels::getLineNumber(std::string_view label) const {
    auto it = labels.find(std::string(label));
    return it != labels.end() ? it->second : -1;
}
