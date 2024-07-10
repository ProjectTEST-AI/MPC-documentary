#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <source_location>
#include <vector>
#include <span>

enum class LogLevel {
    EXCEPTION = -1,
    CRIT = 0,
    WARN = 1,
    HIGH = 2,
    LOW = 3
};

constexpr LogLevel DEFAULT_LOG_LEVEL = LogLevel::WARN;

void setLogLevel(int level);
void log(LogLevel level, std::string_view msg, const std::source_location& location = std::source_location::current());
void flushLogs();
void flushOutput();
void bufferOutput(std::string_view msg);
void setOutputFile(std::string_view filename);

#endif
