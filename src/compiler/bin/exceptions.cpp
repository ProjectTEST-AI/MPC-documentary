#include <iostream>
#include <exception>
#include <string>
#include <chrono>
#include "exceptions.h"
#include "logging.h"

static auto timerStart{ std::chrono::high_resolution_clock::now() }, timerEnd{ std::chrono::high_resolution_clock::now() };

void pauseExit(int code) {
    log(LogLevel::LOW, "Stopping timer");
    timerEnd = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(timerEnd - timerStart);
    std::cout << "\n" << "Transpiler took " << duration.count() << " microseconds of time.\n";
    std::cout << "Closing with code " << code << ", Click enter to exit...";
    std::cin.get();
    std::exit(code);
}

//! syntax errors
InvalidSyntax::InvalidSyntax(const std::string& msg) : message(msg) {}

const char* InvalidSyntax::what() const noexcept {
    return message.c_str();
}

InvalidSyntax InvalidSyntax::emptyCode() {
    return InvalidSyntax("Syntax error: code is empty");
}

InvalidSyntax InvalidSyntax::invalidToken() {
    return InvalidSyntax("Syntax error: invalid token");
}

InvalidSyntax InvalidSyntax::unexpectedEOF() {
    return InvalidSyntax("Syntax error: unexpected end of file");
}

InvalidSyntax InvalidSyntax::invalidInstruction() {
    return InvalidSyntax("Syntax error: invalid instruction");
}

//! file errors
FileError::FileError(const std::string& msg) : message(msg) {}

const char* FileError::what() const noexcept {
    return message.c_str();
}

FileError FileError::noFile() {
    return FileError("File error: File does not exist");
}

// parse func is in compiler.cpp
void parseAndProcess(std::string);

// Wrapper function to handle exceptions
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
        log(LogLevel::EXCEPTION, "Unexpected error : ");
        log(LogLevel::EXCEPTION, e.what());
        pauseExit(99);
    }
}
