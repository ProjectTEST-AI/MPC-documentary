#include "../include/logging.h"
#include "../include/exceptions.h"
#include <iostream>
#include <format>
#include <array>
#include <mutex>
#include <cstdint>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string.h>

void pauseExit(int);

namespace {
    LogLevel currentLogLevel = DEFAULT_LOG_LEVEL;
    std::vector<std::string> logBuffer;
    std::stringstream outputBuffer;
    std::mutex logMutex;
    std::ofstream outputFile;
    constexpr std::size_t MAX_BUFFER_SIZE = 1024 * 1024; // 1 MB

    struct LogConfig {
        const char* prefix;
        std::ostream* stream;
    };

    constexpr std::array<LogConfig, 5> configs = { {
        {"[EXCEPTION] : ", &std::cerr},
        {"[ERROR] : ", &std::cerr},
        {"[WARN] : ", &std::cout},
        {"[LH] : ", &std::clog},
        {"[L1] : ", &std::clog}
    } };

    constexpr const LogConfig& getLogConfig(LogLevel level) {
        return configs[static_cast<int>(level) + 1];
    }
}

void setLogLevel(int level) {
    currentLogLevel = static_cast<LogLevel>(std::clamp(level, 0, 3));
}

void log(LogLevel level, std::string_view msg, const std::source_location& location) {
    if (level <= currentLogLevel) {
        const auto& config = getLogConfig(level);
        std::string logMessage = std::format("{}{}:{} {}\n", config.prefix, level >= LogLevel::WARN ? "" : location.file_name(), location.line(), msg);

        std::lock_guard<std::mutex> lock(logMutex);
        logBuffer.push_back(std::move(logMessage));
    }
}

void flushLogs() {
    std::lock_guard<std::mutex> lock(logMutex);

    // Flush logs
    for (const auto& logMessage : logBuffer) {
        std::cout << logMessage;
    }
    logBuffer.clear();
}

// this thing prone to break fr
void flushOutput() {
    std::lock_guard<std::mutex> lock(logMutex);
    log(LogLevel::LOW, "Entering flushOutput()");

    if (outputFile.is_open()) {
        log(LogLevel::LOW, "Flushing to file");
        outputFile << outputBuffer.str();
        outputFile.flush();
        if (!outputFile) {
            log(LogLevel::CRIT, std::format("File write error occurred. Error: {}", std::strerror(errno)));
            throw FileError::fileWriteError();
        }
    }
    else {
        log(LogLevel::LOW, "Flushing to console");
        std::cout << outputBuffer.str() << std::flush;
    }

    outputBuffer.str("");
    outputBuffer.clear();
    log(LogLevel::LOW, "Exiting flushOutput()");
}

void bufferOutput(std::string_view msg) {
    std::lock_guard<std::mutex> lock(logMutex);
    outputBuffer << msg;
    if (static_cast<std::size_t>(outputBuffer.tellp()) >= MAX_BUFFER_SIZE) {
        flushOutput();
    }
}

void setOutputFile(std::string_view filename) {
    log(LogLevel::LOW, std::format("Attempting to set output file: {}", filename));
    outputFile.open(filename.data(), std::ios::out | std::ios::trunc);
    if (!outputFile.is_open()) {
        log(LogLevel::CRIT, std::format("Failed to open output file: {}. Error: {}", filename, std::strerror(errno)));
        throw FileError::invalidOutputFile();
    }
    outputFile << "Test write\n";
    if (!outputFile) {
        log(LogLevel::CRIT, std::format("Failed to write to output file: {}. Error: {}", filename, std::strerror(errno)));
        throw FileError::fileWriteError();
    }
    outputFile.flush();
    log(LogLevel::LOW, "Output file successfully set and tested");
}