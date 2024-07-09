#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <format>
#include <limits>
#include "../include/exceptions.h"
#include "../include/operations.h"
#include "../include/logging.h"
#include "../include/utils.h"

namespace fs = std::filesystem;

JumpLabels jumpLabels;

void parseInstruction(const std::string& line, int lineNumber) {
    log(LogLevel::LOW, std::format("Entering parseInstruction function for line {}", lineNumber));

    std::string_view trimmedLine = trimComment(line);
    if (isComment(trimmedLine) || trim(trimmedLine).empty()) {
        log(LogLevel::LOW, "Skipping comment or empty line");
        return;
    }

    if (trimmedLine.ends_with(':')) {
        std::string_view label = trim(trimmedLine.substr(0, trimmedLine.size() - 1));
        jumpLabels.addLabel(label, lineNumber);
        log(LogLevel::LOW, std::format("Added jump label: {} at line {}", label, lineNumber));
        return;
    }

    log(LogLevel::LOW, std::format("Parsing instruction: {}", trimmedLine));

    auto operands = split(trimmedLine, ' ');
    operands.shrink_to_fit();

    log(LogLevel::LOW, std::format("Extracted instruction: {} with {} operands", operands[0], operands.size() - 1));

    std::string upperInstruction(operands[0]);
    std::transform(upperInstruction.begin(), upperInstruction.end(), upperInstruction.begin(), ::toupper);

    auto it = Operations::instructionMap.find(upperInstruction);
    if (it == Operations::instructionMap.end()) {
        log(LogLevel::ERROR, std::format("Unknown instruction '{}' on line {}", operands[0], lineNumber));
        throw InvalidSyntax::invalidInstruction();
    }

    if (upperInstruction != operands[0]) {
        log(LogLevel::WARN, std::format("Instruction '{}' is not capitalized on line {}", operands[0], lineNumber));
    }

    const auto& instructionInfo = it->second;

    // Create a span of string_views for the operands, excluding the instruction itself
    std::span<const std::string_view> operandSpan(operands.begin() + 1, operands.end());

    if (!validateOperands(operandSpan, instructionInfo.allowedOperands)) {
        log(LogLevel::ERROR, std::format("Invalid operands for instruction '{}' on line {}", operands[0], lineNumber));
        throw InvalidSyntax::invalidInstruction();
    }

    log(LogLevel::LOW, std::format("Found instruction '{}' with opcode {}", upperInstruction, instructionInfo.opcode));

    log(LogLevel::LOW, "Formatting instruction");
    std::string transpiled = instructionInfo.formatFunc(operands);


    log(LogLevel::HIGH, std::format("Transpiled format: {}", transpiled));
    std::cout << std::move(transpiled) << std::endl;

    log(LogLevel::LOW, "Exiting parseInstruction function");
}

void parseAndProcess(const std::string& sourceFile) {
    log(LogLevel::HIGH, std::format("Parsing and processing file: {}", sourceFile));

    if (!fs::exists(sourceFile)) {
        log(LogLevel::ERROR, std::format("Failed to open file: {}", sourceFile));
        throw FileError::noFile();
    }

    std::ifstream file(sourceFile);
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        try {
            parseInstruction(line, lineNumber);
        }
        catch (const InvalidSyntax& e) {
            log(LogLevel::ERROR, std::format("Syntax error on line {}: {}", lineNumber, e.what()));
            throw;
        }
    }
}

int main() {
    log(LogLevel::HIGH, "Starting transpiler");

    std::string sourceFile;
    int logLevel;

    std::cout << "File Name: ";
    std::getline(std::cin, sourceFile);

    std::cout << "Logging Level (0-3): ";
    std::cin >> logLevel;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    logLevel = std::clamp(logLevel, 0, 3);

    std::cout << std::format("\nInput Summary:\nFile Name: {}\nLogging Level: {} ({})\n\n",
        sourceFile, logLevel,
        (logLevel == 0 ? "ERROR only"
            : logLevel == 1 ? "WARN and up"
            : logLevel == 2 ? "HIGH and up"
            : "All logs"));

    setLogLevel(logLevel);

    try {
        parseAndHandleExceptions(sourceFile);
        log(LogLevel::HIGH, "Transpiler finished successfully");
        pauseExit(0);
    }
    catch (const std::exception& e) {
        log(LogLevel::EXCEPTION, std::format("Main process failed to run: {}", e.what()));
        pauseExit(99);
    }
}