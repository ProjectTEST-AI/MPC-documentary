#ifndef LOGGING_H
#define LOGGING_H

#include <string>

enum class LogLevel {
    ERROR = 0,
    WARN = 1,
    HIGH = 2,
    LOW = 3
};

void setLogLevel(int level);
void log(LogLevel level, const std::string& msg);

#endif
