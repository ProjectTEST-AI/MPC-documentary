#include <iostream>
#include <exception>
#include <string>
#include "exceptions.h"
#include "logging.h"

void pauseExit(int code) {
    std::cout << "\n" << "Closing with code " << code << ", Click enter to exit...";
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
        log(LogLevel::LOW, "process reached main exception handler");
        parseAndProcess(file);
    }
    catch (const InvalidSyntax& e) {
        log(LogLevel::ERROR, e.what());
        pauseExit(1);
    }
    catch (const FileError& e) {
        log(LogLevel::ERROR, e.what());
        pauseExit(2);
    }
    catch (const std::exception& e) {
        log(LogLevel::ERROR, "Unexpected error : ");
        log(LogLevel::ERROR, e.what());
        pauseExit(99);
    }
}
