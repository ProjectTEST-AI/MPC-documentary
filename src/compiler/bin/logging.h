#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <source_location>

enum class LogLevel {
    EXCEPTION = -1,
    ERROR = 0,
    WARN = 1,
    HIGH = 2,
    LOW = 3
};

constexpr LogLevel DEFAULT_LOG_LEVEL = LogLevel::WARN;

void setLogLevel(int level);
void log(LogLevel level, const std::string& msg, const std::source_location& location = std::source_location::current());

#endif
