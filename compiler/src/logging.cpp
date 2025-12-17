#include <mutex>
#include <array>
#include <format>
#include <vector>
#include <fstream>
#include <numeric>
#include <iostream>
#include <algorithm>
#include "../include/logging.h"
#include "../include/exceptions.h"

void pauseExit(int);

namespace {
    LogLevel currentLogLevel = DEFAULT_LOG_LEVEL;
    std::vector<std::string> logBuffer;
    std::string outputFileName;
    std::vector<std::string> outputBuffer;
    
    // Separate mutexes to prevent deadlock
    std::mutex logMutex;
    std::mutex outputMutex;
    
    constexpr std::size_t MAX_BUFFER_SIZE = 500 * 1024 * 1024; // 500 MB

    struct LogConfig {
        const char* prefix;
        std::ostream* stream;
    };

    constexpr std::array<LogConfig, 4> configs = { {
        {"[CRITICAL] ", &std::cerr},
        {"[ERROR] ", &std::cerr},
        {"[WARN] ", &std::cout},
        {"[INFO] ", &std::clog}
    } };

    constexpr const LogConfig& getLogConfig(LogLevel level) {
        return configs[static_cast<int>(level)];
    }

    void logImpl(LogLevel level, std::string_view msg, bool exc_info,
                 const std::source_location& location) {
        if (level <= currentLogLevel) {
            const auto& config = getLogConfig(level);
            std::string logMessage;
            
            if (exc_info) {
                // Include file and line info for exceptions
                logMessage = std::format("{}{}:{} {}\n", 
                    config.prefix, location.file_name(), location.line(), msg);
            } else {
                logMessage = std::format("{}{}\n", config.prefix, msg);
            }

            std::lock_guard lock(logMutex);
            logBuffer.push_back(std::move(logMessage));
        }
    }
}

// Global logger instance
const Logger log;

void Logger::setLevel(int level) const {
    currentLogLevel = static_cast<LogLevel>(std::clamp(level, 0, 3));
}

void Logger::info(std::string_view msg, const std::source_location& location) const {
    logImpl(LogLevel::INFO, msg, false, location);
}

void Logger::warn(std::string_view msg, const std::source_location& location) const {
    logImpl(LogLevel::WARN, msg, false, location);
}

void Logger::error(std::string_view msg, bool exc_info, const std::source_location& location) const {
    logImpl(LogLevel::ERROR, msg, exc_info, location);
}

void Logger::critical(std::string_view msg, bool exc_info, const std::source_location& location) const {
    logImpl(LogLevel::CRITICAL, msg, exc_info, location);
}

void Logger::flush() const {
    std::lock_guard lock(logMutex);

    for (const auto& logMessage : logBuffer) {
        std::cout << logMessage;
    }
    std::cout.flush();

    logBuffer.clear();
}

void setOutputFile(std::string_view filename) {
    {
        std::lock_guard lock(outputMutex);
        outputFileName = filename;
    }
    // Log AFTER releasing the lock
    log.info(std::format("Output file set to: {}", filename.empty() ? "console" : filename));
}

void bufferOutput(std::string_view msg) {
    std::lock_guard lock(outputMutex);
    outputBuffer.emplace_back(msg);

    size_t totalSize = std::accumulate(outputBuffer.begin(), outputBuffer.end(), 0ULL,
        [](size_t sum, const std::string_view& s) { return sum + s.size(); });

    if (totalSize > MAX_BUFFER_SIZE) {
        // Can't call log here while holding lock, throw directly
        throw OutputBufferError::overLimit(MAX_BUFFER_SIZE, totalSize);
    }
}

void flushOutput() {
    std::string fileName;
    std::vector<std::string> buffer;
    
    // Copy data under lock, then release
    {
        std::lock_guard lock(outputMutex);
        fileName = outputFileName;
        buffer = std::move(outputBuffer);
        outputBuffer.clear();
    }

    // Now process without holding the lock
    if (fileName.empty()) {
        for (const auto& line : buffer) {
            std::cout << line;
        }
        std::cout.flush();
    }
    else {
        std::ofstream outFile(fileName, std::ios::out | std::ios::app);
        if (!outFile.is_open()) {
            log.critical(std::format("Failed to open output file: {}.", fileName));
            throw FileError::invalidOutputFile();
        }

        for (const auto& line : buffer) {
            outFile << line;
        }
        outFile.flush();

        if (!outFile.good()) {
            log.critical(std::format("Failed to write to output file: {}.", fileName));
            throw FileError::fileWriteError();
        }
    }

    log.info("Output buffer flushed.");
}

void resetOutputBuffer() {
    {
        std::lock_guard lock(outputMutex);
        outputBuffer.clear();
    }
    log.info("Output buffer cleared.");
}
