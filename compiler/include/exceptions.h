#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

// Syntax errors
class InvalidSyntax : public std::runtime_error {
public:
    explicit InvalidSyntax(const std::string& msg) : std::runtime_error(msg) {}

    static InvalidSyntax emptyCode() { return InvalidSyntax("Syntax error: code is empty"); };
    static InvalidSyntax invalidToken() { return InvalidSyntax("Syntax error: invalid token"); };
    static InvalidSyntax unexpectedEOF() { return InvalidSyntax("Syntax error: unexpected end of file"); };
    static InvalidSyntax invalidInstruction() { return InvalidSyntax("Syntax error: invalid instruction"); };
};

// File errors
class FileError : public std::runtime_error {
public:
    explicit FileError(const std::string& msg) : std::runtime_error(msg) {}

    static FileError noFile() { return FileError("File error: File does not exist"); };
    static FileError invalidOutputFile() { return FileError("File error: Output file cannot be opened/created"); };
    static FileError fileWriteError() { return FileError("File error: Failed to write into specified output file"); }
};

#endif
