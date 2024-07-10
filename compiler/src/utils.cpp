#include "../include/utils.h"
#include <cctype>
#include <format>

std::string_view stripSymbols(std::string_view value) noexcept {
    auto it = std::ranges::find_if(value, [](char c) { return std::isdigit(c) != 0; });
    return std::string_view(it, value.end());
}

std::string formatInstruction(int opcode, std::span<const int> dataTypes, std::span<const int> negatives, std::span<const std::string_view> rawValues) {
    return std::format("{} {} {} {} {} {} {}", opcode, dataTypes[0], dataTypes[1], negatives[0], negatives[1], rawValues[0], rawValues[1]);
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
