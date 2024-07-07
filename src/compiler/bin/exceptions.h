#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

// function that handles exit codes
void pauseExit(int code = 0);

// Function for the wrapper that handles exceptions
void parseAndHandleExceptions(const std::string& file);

// syntax errors
class InvalidSyntax : public std::exception {
private:
    std::string message;

public:
    explicit InvalidSyntax(const std::string& msg);
    virtual const char* what() const noexcept override;

    static InvalidSyntax emptyCode();
    static InvalidSyntax invalidToken();
    static InvalidSyntax unexpectedEOF();
    static InvalidSyntax invalidInstruction();
};

// file errors
class FileError : public std::exception {
private:
    std::string message;

public:
    explicit FileError(const std::string& msg);
    virtual const char* what() const noexcept override;

    static FileError noFile();
};

#endif
