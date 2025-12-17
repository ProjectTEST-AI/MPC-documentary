#pragma once

#include <format>
#include <string>
#include <stdexcept>

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

// Operand validation errors
class InvalidOperands : public std::runtime_error {
public:
    explicit InvalidOperands(const std::string& msg) : std::runtime_error(msg) {}

    static InvalidOperands mismatchCount(const std::string& instruction, size_t expected, size_t got) {
        return InvalidOperands(std::format("Operand error: instruction '{}' expects {} operand(s), got {}", instruction, expected, got));
    }

    static InvalidOperands invalidType(const std::string& instruction, size_t position, const std::string& expected, const std::string& got) {
        return InvalidOperands(std::format("Operand error: instruction '{}' operand {} expects {}, got {}", instruction, position, expected, got));
    }
};

// Output buffer errors
class OutputBufferError : public std::runtime_error {
public:
    explicit OutputBufferError(const std::string& msg) : std::runtime_error(msg) {}

    static OutputBufferError overLimit(std::size_t limitBytes, std::size_t attemptedBytes) {
        return OutputBufferError(std::format(
            "Output buffer exceeded limit: {} bytes used, limit {} bytes", attemptedBytes, limitBytes));
    }
};
