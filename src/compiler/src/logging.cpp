#include <iostream>
#include <array>
#include <format>
#include "../include/logging.h"

namespace {
    LogLevel currentLogLevel = DEFAULT_LOG_LEVEL;

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
    currentLogLevel = static_cast<LogLevel>(std::min(std::max(level, 0), 3));
}

void log(LogLevel level, const std::string& msg, const std::source_location& location) {
    if (level <= currentLogLevel) {
        const auto& config = getLogConfig(level);
        *config.stream << std::format("{}{}:{} {}\n", config.prefix, location.file_name(), location.line(), msg);
    }
}
