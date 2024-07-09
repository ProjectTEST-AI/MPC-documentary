#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

// Function that handles exit codes
void pauseExit(int code = 0);

// Function for the wrapper that handles exceptions
void parseAndHandleExceptions(const std::string& file);

// Syntax errors
class InvalidSyntax : public std::runtime_error {
public:
    explicit InvalidSyntax(const std::string& msg);

    static InvalidSyntax emptyCode();
    static InvalidSyntax invalidToken();
    static InvalidSyntax unexpectedEOF();
    static InvalidSyntax invalidInstruction();
};

// File errors
class FileError : public std::runtime_error {
public:
    explicit FileError(const std::string& msg);

    static FileError noFile();
};

// Declaration of parseAndProcess function
void parseAndProcess(const std::string& sourceFile);

#endif
