#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include "bin/exceptions.h"
#include "bin/operations.h"
#include "bin/logging.h"

// Helper function to determine the data type
int getDataType(const std::string& value) {
    log(LogLevel::LOW, "Determining data type for value: " + value);
    if (value[0] == '@' || (value[0] == '-' && value[1] == '@')) {
        log(LogLevel::LOW, "Data type: Memory ID (1)");
        return 1; // Memory ID
    }
    if (value[0] == '#' || (value[0] == '-' && value[1] == '#')) {
        log(LogLevel::LOW, "Data type: Register ID (0)");
        return 0; // Register ID
    }
    log(LogLevel::LOW, "Data type: Regular value (2)");
    return 2; // Regular value
}

// Helper function to determine if the value is negative
bool isNegative(const std::string& value) {
    log(LogLevel::LOW, "Checking if value is negative: " + value);
    bool result = value[0] == '-';
    log(LogLevel::LOW, result ? "Value is negative" : "Value is positive");
    return result;
}

// Helper function to strip symbols and get the raw value
std::string stripSymbols(const std::string& value) {
    log(LogLevel::LOW, "Stripping symbols from value: " + value);
    std::string stripped;
    for (char c : value) {
        if (std::isdigit(c)) {
            stripped += c;
        }
    }
    log(LogLevel::LOW, "Stripped value: " + stripped);
    return stripped;
}

void parseInstruction(const std::string& line, int lineNumber) {
    log(LogLevel::HIGH, "Parsing instruction on line " + std::to_string(lineNumber) + ": " + line);
    std::istringstream iss(line);
    std::string instruction, value1, value2;

    // Parse instruction
    iss >> instruction;

    // Parse the rest of the line
    std::string rest;
    std::getline(iss, rest);

    // Find the position of the comma
    size_t commaPos = rest.find(',');
    if (commaPos == std::string::npos) {
        log(LogLevel::ERROR, "Invalid instruction format: missing comma");
        throw InvalidSyntax::invalidInstruction();
    }

    // Split the rest into value1 and value2
    value1 = rest.substr(0, commaPos);
    value2 = rest.substr(commaPos + 1);

    // Trim whitespace
    value1.erase(0, value1.find_first_not_of(" \t"));
    value1.erase(value1.find_last_not_of(" \t") + 1);
    value2.erase(0, value2.find_first_not_of(" \t"));
    value2.erase(value2.find_last_not_of(" \t") + 1);

    log(LogLevel::LOW, "Parsed components: instruction=" + instruction + ", value1=" + value1 + ", value2=" + value2);

    if (instruction.empty() || value1.empty() || value2.empty()) {
        log(LogLevel::ERROR, "Invalid instruction format");
        throw InvalidSyntax::invalidInstruction();
    }

    int opcode;
    if (instruction == "ADD") {
        opcode = 1;
        log(LogLevel::LOW, "Instruction identified as ADD (opcode 1)");
    }
    else if (instruction == "SUB") {
        opcode = 2;
        log(LogLevel::LOW, "Instruction identified as SUB (opcode 2)");
    }
    else {
        log(LogLevel::ERROR, "Unknown instruction: " + instruction);
        throw InvalidSyntax::invalidInstruction();
    }

    int dataType1 = getDataType(value1);
    int dataType2 = getDataType(value2);
    int negative1 = isNegative(value1) ? 1 : 0;
    int negative2 = isNegative(value2) ? 1 : 0;
    std::string rawValue1 = stripSymbols(value1);
    std::string rawValue2 = stripSymbols(value2);

    // Output the transpiled format
    std::string transpiled = std::to_string(opcode) + " " +
        std::to_string(dataType1) + " " +
        std::to_string(dataType2) + " " +
        std::to_string(negative1) + " " +
        std::to_string(negative2) + " " +
        rawValue1 + " " +
        rawValue2;
    log(LogLevel::HIGH, "Transpiled format: " + transpiled);
    std::cout << transpiled << std::endl;

    // Call the appropriate operation
    log(LogLevel::LOW, "Attempting to execute operation");
    auto it = Operations::instructionMap.find(instruction);
    if (it != Operations::instructionMap.end()) {
        log(LogLevel::LOW, "Executing " + instruction + " operation");
        it->second(value1, value2);
    }
    else {
        log(LogLevel::ERROR, "No implementation found for instruction: " + instruction);
        throw InvalidSyntax::invalidInstruction();
    }
}

void parseAndProcess(const std::string sourceFile) {
    log(LogLevel::HIGH, "Opening File: " + sourceFile);
    std::ifstream file(sourceFile);
    log(LogLevel::LOW, "Checking file existence..");
    if (!file.is_open()) {
        log(LogLevel::ERROR, "File not found: " + sourceFile);
        throw FileError::noFile();
    }
    log(LogLevel::LOW, "File exists, proceeding");

    std::string line;
    int lineNumber = 0;
    log(LogLevel::HIGH, "Starting to parse file contents");
    while (std::getline(file, line)) {
        lineNumber++;
        log(LogLevel::LOW, "Parsing Line " + std::to_string(lineNumber) + ": " + line);
        try {
            parseInstruction(line, lineNumber);
            log(LogLevel::LOW, "Successfully processed line " + std::to_string(lineNumber));
        }
        catch (const InvalidSyntax& e) {
            log(LogLevel::ERROR, "Syntax error on line " + std::to_string(lineNumber) + ": " + e.what());
            throw;
        }
    }

    log(LogLevel::HIGH, "Finished parsing file. Closing file.");
    file.close();
}

int main() {
    log(LogLevel::HIGH, "Program started");
    std::string sourceFile;
    int logLevel;

    std::cout << "File Name: ";
    std::getline(std::cin, sourceFile);
    log(LogLevel::LOW, "User entered file name: " + sourceFile);

    std::cout << "Logging Level (0-3): ";
    std::cin >> logLevel;
    log(LogLevel::LOW, "User entered log level: " + std::to_string(logLevel));

    // Clear the input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Validate log level input
    if (logLevel < 0 || logLevel > 3) {
        log(LogLevel::WARN, "Invalid log level entered. Defaulting to 0 (ERROR only)");
        std::cerr << "Invalid log level. Using default (0: ERROR only).\n";
        logLevel = 0;
    }

    // Display the input summary
    std::cout << "\nInput Summary:\n";
    std::cout << "File Name: " << sourceFile << "\n";
    std::cout << "Logging Level: " << logLevel << " (";
    switch (logLevel) {
    case 0: std::cout << "ERROR only";   break;
    case 1: std::cout << "WARN and up";  break;
    case 2: std::cout << "HIGH and up";  break;
    case 3: std::cout << "All logs";     break;
    }
    std::cout << ")\n\n";

    log(LogLevel::HIGH, "Setting log level to " + std::to_string(logLevel));
    setLogLevel(logLevel);

    try {
        log(LogLevel::HIGH, "Starting main process");
        parseAndHandleExceptions(sourceFile);
        log(LogLevel::LOW, "Main process completed successfully");
    }
    catch (const std::exception& e) {
        log(LogLevel::EXCEPTION, "Main process failed to run: " + std::string(e.what()));
        pauseExit(99);
    }

    log(LogLevel::HIGH, "Sucessfully executed, exitting.");
    pauseExit(0);
}
