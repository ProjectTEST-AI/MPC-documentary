#include <iostream>
#include "logging.h"

static LogLevel currentLogLevel = LogLevel::ERROR;

void setLogLevel(int level) {
    switch (level) {
    case 0:
        currentLogLevel = LogLevel::ERROR;
        break;
    case 1:
        currentLogLevel = LogLevel::WARN;
        break;
    case 2:
        currentLogLevel = LogLevel::HIGH;
        break;
    case 3:
        currentLogLevel = LogLevel::LOW;
        break;
    default:
        currentLogLevel = LogLevel::ERROR;
        break;
    }
}

void log(LogLevel level, const std::string& msg) {
    if (level <= currentLogLevel) {
        switch (level) {
        case LogLevel::LOW:
            std::clog << "[L1] : " << msg << "\n";
            break;
        case LogLevel::HIGH:
            std::clog << "[LH] : " << msg << "\n";
            break;
        case LogLevel::WARN:
            std::cout << "[WARN] : " << msg << "\n";
            break;
        case LogLevel::ERROR:
            std::cerr << "[ERROR] : " << msg << "\n";
            break;
        }
    }
}
