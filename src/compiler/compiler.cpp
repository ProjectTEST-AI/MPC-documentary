#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <format>
#include <limits>
#include "bin/exceptions.h"
#include "bin/operations.h"
#include "bin/logging.h"
#include "bin/utils.h"

namespace fs = std::filesystem;

void parseInstruction(std::string_view line, int lineNumber) {
    log(LogLevel::LOW, std::format("Parsing instruction on line {}: {}", lineNumber, line));

    auto operands = split(line, ' ');

    auto it = Operations::instructionMap.find(operands[0]);
    if (it == Operations::instructionMap.end()) {
        log(LogLevel::ERROR, std::format("Unknown instruction '{}' on line {}", operands[0], lineNumber));
        throw InvalidSyntax::invalidInstruction();
    }

    const auto& instructionInfo = it->second;

    if (operands.size() - 1 != instructionInfo.operandCount) {
        log(LogLevel::ERROR, std::format("Invalid number of operands for instruction '{}' on line {}", operands[0], lineNumber));
        throw InvalidSyntax::invalidInstruction();
    }

    std::vector<std::string_view> operandViews;
    operandViews.reserve(operands.size());
    for (const auto& op : operands) {
        operandViews.push_back(op);
    }

    std::string transpiled = instructionInfo.formatFunc(operandViews);

    log(LogLevel::HIGH, std::format("Transpiled format: {}", transpiled));
    std::cout << transpiled << '\n';
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