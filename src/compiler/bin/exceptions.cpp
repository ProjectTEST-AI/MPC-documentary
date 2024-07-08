#include <iostream>
#include <chrono>
#include "exceptions.h"
#include "logging.h"

namespace {
    auto timerStart = std::chrono::high_resolution_clock::now();
    auto timerEnd = std::chrono::high_resolution_clock::now();
}

void pauseExit(int code) {
    log(LogLevel::LOW, "Stopping timer");
    timerEnd = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(timerEnd - timerStart);
    std::cout << "\nTranspiler took " << duration.count() << " microseconds.\n";
    std::cout << "Exiting with code " << code << " || ";
    std::system("pause");
    std::exit(code);
}

InvalidSyntax::InvalidSyntax(const std::string& msg) : std::runtime_error(msg) {}

InvalidSyntax InvalidSyntax::emptyCode() { return InvalidSyntax("Syntax error: code is empty"); }
InvalidSyntax InvalidSyntax::invalidToken() { return InvalidSyntax("Syntax error: invalid token"); }
InvalidSyntax InvalidSyntax::unexpectedEOF() { return InvalidSyntax("Syntax error: unexpected end of file"); }
InvalidSyntax InvalidSyntax::invalidInstruction() { return InvalidSyntax("Syntax error: invalid instruction"); }

FileError::FileError(const std::string& msg) : std::runtime_error(msg) {}

FileError FileError::noFile() { return FileError("File error: File does not exist"); }

void parseAndHandleExceptions(const std::string& file) {
    try {
        log(LogLevel::LOW, "Process reached main exception handler");
        log(LogLevel::LOW, "Starting timer");
        timerStart = std::chrono::high_resolution_clock::now();
        parseAndProcess(file);
    }
    catch (const InvalidSyntax& e) {
        log(LogLevel::EXCEPTION, e.what());
        pauseExit(1);
    }
    catch (const FileError& e) {
        log(LogLevel::EXCEPTION, e.what());
        pauseExit(2);
    }
    catch (const std::exception& e) {
        log(LogLevel::EXCEPTION, std::string("Unexpected error: ") + e.what());
        pauseExit(99);
    }
}
