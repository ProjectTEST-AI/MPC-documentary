#include "../include/logging.h"
#include "../include/exceptions.h"
#include <iostream>
#include <format>
#include <array>
#include <mutex>
#include <cstdint>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <string.h>

void pauseExit(int);

namespace {
    LogLevel currentLogLevel = DEFAULT_LOG_LEVEL;
    std::vector<std::string> logBuffer;
    std::string outputFileName;
    std::vector<std::string> outputBuffer;
    std::mutex logMutex;
    constexpr std::size_t MAX_BUFFER_SIZE = 500 * 1024 * 1024; // 500 MB

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

    log(LogLevel::HIGH, "Clearing logs buffer..");
    logBuffer.clear();
}

void setOutputFile(std::string_view filename) {
    std::lock_guard<std::mutex> lock(logMutex);
    outputFileName = filename;
    log(LogLevel::LOW, std::format("Output file set to: {}", filename.empty() ? "console" : filename));
}

void bufferOutput(std::string_view msg) {
    std::lock_guard<std::mutex> lock(logMutex);
    outputBuffer.push_back(std::string(msg));

    size_t totalSize = std::accumulate(outputBuffer.begin(), outputBuffer.end(), 0ULL,
        [](size_t sum, const std::string& s) { return sum + s.size(); });

    if (totalSize > MAX_BUFFER_SIZE) {
        log(LogLevel::CRIT, "Output buffer size exceeded 500MB limit");
        throw std::runtime_error("Output buffer size exceeded 500MB limit");
    }
}

void flushOutput() {
    std::lock_guard<std::mutex> lock(logMutex);

    if (outputFileName.empty()) {
        // Output to console
        for (const auto& line : outputBuffer) {
            std::cout << line;
        }
        std::cout.flush();
    }
    else {
        // Output to file
        std::ofstream outFile(outputFileName, std::ios::out | std::ios::app);
        if (!outFile.is_open()) {
            log(LogLevel::CRIT, std::format("Failed to open output file: {}.", outputFileName));
            throw FileError::invalidOutputFile();
        }

        for (const auto& line : outputBuffer) {
            outFile << line;
        }
        outFile.flush();

        if (!outFile.good()) {
            log(LogLevel::CRIT, std::format("Failed to write to output file: {}.", outputFileName));
            throw FileError::fileWriteError();
        }
    }

    // Clear the buffer after successful write
    log(LogLevel::HIGH, "Clearing output buffer..");
    outputBuffer.clear();
}

void resetOutputBuffer() {
    log(LogLevel::HIGH, "Clearing output buffer..");
    outputBuffer.clear();
}
