#pragma once

#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <optional>
#include "utils.h"
#include "exceptions.h"
#include "memory_mapped_file.hpp"

// Source Abstraction for Streaming
struct LineSource {
    virtual std::optional<std::string> next() = 0;
    virtual ~LineSource() = default;
};

struct StringSource : LineSource {
    std::stringstream ss;
    explicit StringSource(const std::string& str) : ss(str) {}
    std::optional<std::string> next() override {
        std::string line;
        if (std::getline(ss, line)) return line;
        return std::nullopt;
    }
};

struct FileSource : LineSource {
    std::ifstream file;
    explicit FileSource(const std::string& path) : file(path) {
        if (!file) throw FileError(std::format("Cannot open file: {}", path));
    }
    std::optional<std::string> next() override {
        std::string line;
        if (std::getline(file, line)) return line;
        return std::nullopt;
    }
};

struct MmapSource : LineSource {
    MemoryMappedFile mmap;
    const char* cur;
    const char* end;
    
    explicit MmapSource(const std::string& path) : mmap(path) {
        auto span = mmap.get_span();
        cur = span.data();
        end = cur + span.size();
    }
    
    std::optional<std::string> next() override {
        if (cur >= end) return std::nullopt;
        const char* newline = std::find(cur, end, '\n');
        std::string line(cur, newline); 
        cur = (newline < end) ? newline + 1 : end;
        return line;
    }
};

// Create appropriate LineSource based on config
[[nodiscard]] std::unique_ptr<LineSource> createLineSource(const CompilerConfig& config);

// Multi-threaded processing (works for both compile and decompile)
void compileMultiThread(const CompilerConfig& config, LineSource& source);
void decompileMultiThread(const CompilerConfig& config, LineSource& source);
