#pragma once

#include <string_view>
#include <source_location>

enum class LogLevel { CRITICAL = 0, ERROR = 1, WARN = 2, INFO = 3 };

constexpr LogLevel DEFAULT_LOG_LEVEL = LogLevel::WARN;

class Logger {
public:
    void setLevel(int level) const;

    void info(std::string_view msg,
              const std::source_location& location = std::source_location::current()) const;

    void warn(std::string_view msg,
              const std::source_location& location = std::source_location::current()) const;

    void error(std::string_view msg, bool exc_info = false,
               const std::source_location& location = std::source_location::current()) const;

    void critical(std::string_view msg, bool exc_info = true,
                  const std::source_location& location = std::source_location::current()) const;

    void flush() const;
};

// Global logger instance
extern const Logger log;

// Output utilities
void flushOutput();
void resetOutputBuffer();
void bufferOutput(std::string_view msg);
void setOutputFile(std::string_view filename);
