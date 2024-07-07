#include "operations.h"
#include "logging.h"

std::unordered_map<std::string, int> Operations::registers;

const std::unordered_map<std::string, std::function<void(const std::string&, const std::string&)>> Operations::instructionMap = {
    {"ADD", Operations::add},
    {"SUB", Operations::sub}
    // TODO: Add more operations
};

void Operations::add(const std::string& reg1, const std::string& reg2) {
    log(LogLevel::LOW, "Performing ADD operation: " + reg1 + " + " + reg2);
    log(LogLevel::LOW, "Before: " + reg1 + " = " + std::to_string(registers[reg1]) + ", " + reg2 + " = " + std::to_string(registers[reg2]));
    registers[reg1] += registers[reg2];
    log(LogLevel::LOW, "After: " + reg1 + " = " + std::to_string(registers[reg1]));
    log(LogLevel::HIGH, "Added #" + reg2 + " to #" + reg1);
}

void Operations::sub(const std::string& reg1, const std::string& reg2) {
    log(LogLevel::LOW, "Performing SUB operation: " + reg1 + " - " + reg2);
    log(LogLevel::LOW, "Before: " + reg1 + " = " + std::to_string(registers[reg1]) + ", " + reg2 + " = " + std::to_string(registers[reg2]));
    registers[reg1] -= registers[reg2];
    log(LogLevel::LOW, "After: " + reg1 + " = " + std::to_string(registers[reg1]));
    log(LogLevel::HIGH, "Subtracted #" + reg2 + " from #" + reg1);
}
