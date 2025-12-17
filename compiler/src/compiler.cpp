#include <span>
#include <format>
#include <algorithm>
#include "../include/logging.h"
#include "../include/compiler.h"
#include "../include/operations.h"
#include "../include/multithreading.h"

std::string parseInstruction(std::string_view lineView, size_t lineNumber, JumpLabels& jumpLabels) {
    std::string_view trimmedLine = trimComment(lineView);
    if (isComment(trimmedLine) || trim(trimmedLine).empty()) {
        return {};
    }

    if (trimmedLine.ends_with(':')) {
        std::string_view label = trim(trimmedLine.substr(0, trimmedLine.size() - 1));
        jumpLabels.addLabel(label, static_cast<int>(lineNumber));
        return {};
    }

    auto operands = split(trimmedLine, ' ');
    if (operands.empty()) return {};

    std::string_view instructionView = operands[0];
    auto it = Operations::instructionMap.find(instructionView);
    if (it == Operations::instructionMap.end()) {
        std::string upperInstruction(instructionView);
        std::ranges::transform(upperInstruction, upperInstruction.begin(), ::toupper);
        it = Operations::instructionMap.find(upperInstruction);
        if (it == Operations::instructionMap.end()) {
            log.warn(std::format("Unknown instruction '{}' on line {}, skipping", instructionView, lineNumber));
            return {}; 
        }
    }

    const auto& instructionInfo = it->second;

    if (std::span<const std::string_view> operandSpan(operands.begin() + 1, operands.end());
        !validateOperands(operandSpan, instructionInfo.allowedOperands)) {
        log.warn(std::format("Invalid operands for instruction '{}' on line {}, skipping", instructionView, lineNumber));
        return {};
    }

    return instructionInfo.formatFunc(operands) + '\n';
}

void compileSingleThread(LineSource& source) {
    JumpLabels jumpLabels;
    size_t lineNumber = 1;
    while (auto line = source.next()) {
        std::string result = parseInstruction(*line, lineNumber++, jumpLabels);
        if (!result.empty()) {
            bufferOutput(std::move(result));
        }
    }
}
