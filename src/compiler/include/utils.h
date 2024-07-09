#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string_view>
#include <array>
#include <span>
#include <vector>
#include <algorithm>
#include <unordered_map>

[[nodiscard]] inline constexpr int getDataType(std::string_view value) noexcept {
    if (value.empty()) return 2;
    if (value[0] == '@' || (value[0] == '-' && value.size() > 1 && value[1] == '@')) return 1; // Memory ID
    if (value[0] == '#' || (value[0] == '-' && value.size() > 1 && value[1] == '#')) return 0; // Register ID
    return 2; // Regular value
}

[[nodiscard]] inline constexpr bool isNegative(std::string_view value) noexcept {
    return !value.empty() && value[0] == '-';
}

[[nodiscard]] constexpr std::string_view trimComment(std::string_view line) noexcept {
    size_t commentPos = line.find('/');
    return commentPos != std::string_view::npos ? line.substr(0, commentPos) : line;
}

[[nodiscard]] constexpr bool isComment(std::string_view line) noexcept {
    return !line.empty() && line[0] == '/';
}

[[nodiscard]] constexpr std::string_view trim(std::string_view s) noexcept {
    auto isSpace = [](unsigned char c) { return std::isspace(c); };
    auto start = std::find_if_not(s.begin(), s.end(), isSpace);
    auto end = std::find_if_not(s.rbegin(), std::string_view::reverse_iterator(start), isSpace).base();
    return std::string_view(&*start, end - start);
}

[[nodiscard]] std::string_view stripSymbols(std::string_view value) noexcept;
[[nodiscard]] std::string formatInstruction(int opcode, std::span<const int> dataTypes, std::span<const int> negatives, std::span<const std::string_view> rawValues);
[[nodiscard]] std::vector<std::string_view> split(std::string_view s, char delimiter);

class JumpLabels {
public:
    void addLabel(std::string_view label, int lineNumber);
    [[nodiscard]] int getLineNumber(std::string_view label) const;
private:
    std::unordered_map<std::string, int> labels;
};

#endif
