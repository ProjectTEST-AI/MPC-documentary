#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <string>
#include <unordered_map>
#include <functional>

class Operations {
public:
    static void add(const std::string& reg1, const std::string& reg2);
    static void sub(const std::string& reg1, const std::string& reg2);
    // TODO: Add more operations

    static const std::unordered_map<std::string, std::function<void(const std::string&, const std::string&)>> instructionMap;

private:
    static std::unordered_map<std::string, int> registers;
};

#endif
