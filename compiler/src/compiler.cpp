#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <format>
#include <limits>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <atomic>
#include <future>
#include <ranges>
#include <span>
#include <latch>
#include <barrier>
#include <numeric>
#include <semaphore>
#include "../include/memory_mapped_file.hpp"
#include "../include/exceptions.h"
#include "../include/operations.h"
#include "../include/logging.h"
#include "../include/utils.h"

JumpLabels jumpLabels;

std::string parseInstruction(std::span<const char> line, size_t lineNumber) {
    log(LogLevel::LOW, std::format("Entering parseInstruction function for line {}", lineNumber));

    std::string_view lineView(line.data(), line.size());
    std::string_view trimmedLine = trimComment(lineView);
    if (isComment(trimmedLine) || trim(trimmedLine).empty()) {
        log(LogLevel::LOW, "Skipping comment or empty line");
        return "";
    }

    if (trimmedLine.ends_with(':')) {
        std::string_view label = trim(trimmedLine.substr(0, trimmedLine.size() - 1));
        jumpLabels.addLabel(label, static_cast<int>(lineNumber));
        log(LogLevel::LOW, std::format("Added jump label: {} at line {}", label, lineNumber));
        return "";
    }

    log(LogLevel::LOW, std::format("Parsing instruction: {}", trimmedLine));

    auto operands = split(trimmedLine, ' ');
    operands.shrink_to_fit();

    log(LogLevel::LOW, std::format("Extracted instruction: {} with {} operands", operands[0], operands.size() - 1));

    std::string upperInstruction(operands[0]);
    std::ranges::transform(upperInstruction, upperInstruction.begin(), ::toupper);

    auto it = Operations::instructionMap.find(upperInstruction);
    if (it == Operations::instructionMap.end()) {
        log(LogLevel::CRIT, std::format("Unknown instruction '{}' on line {}", operands[0], lineNumber));
        throw InvalidSyntax::invalidInstruction();
    }

    if (upperInstruction != operands[0]) {
        log(LogLevel::WARN, std::format("Instruction '{}' is not capitalized on line {}", operands[0], lineNumber));
    }

    const auto& instructionInfo = it->second;

    // Create a span of string_views for the operands, excluding the instruction itself
    std::span<const std::string_view> operandSpan(operands.begin() + 1, operands.end());

    if (!validateOperands(operandSpan, instructionInfo.allowedOperands)) {
        log(LogLevel::CRIT, std::format("Invalid operands for instruction '{}' on line {}", operands[0], lineNumber));
        throw InvalidSyntax::invalidInstruction();
    }

    log(LogLevel::LOW, std::format("Found instruction '{}' with opcode {}", upperInstruction, instructionInfo.opcode));

    log(LogLevel::LOW, "Formatting instruction");
    std::string transpiled = instructionInfo.formatFunc(operands);
    log(LogLevel::HIGH, std::format("Transpiled format: {}", transpiled));

    log(LogLevel::LOW, "Exiting parseInstruction function");
    return transpiled;
}

void parseAndProcess(std::string_view sourceFile, std::string_view outputFile, bool useMultithreading) {
    try {
        MemoryMappedFile mmapFile(sourceFile);
        std::span fileContent = mmapFile.get_span();

        if (!outputFile.empty()) {
            setOutputFile(outputFile);
        }

        std::vector<std::pair<size_t, std::string_view>> lines;
        size_t lineNumber = 1;
        const char* begin = fileContent.data();
        const char* end = begin + fileContent.size();
        while (begin < end) {
            const char* newline = std::find(begin, end, '\n');
            lines.emplace_back(lineNumber++, std::string_view(begin, newline - begin));
            begin = newline + 1;
        }

        if (useMultithreading) {
            unsigned int threadCount = std::max(1U, std::thread::hardware_concurrency());
            std::atomic<size_t> lineIndex(0);
            std::atomic<int> activeThreads(threadCount);

            auto worker = [&]() {
                try {
                    std::vector<std::string> localBuffer;
                    while (true) {
                        size_t index = lineIndex.fetch_add(1, std::memory_order_relaxed);
                        if (index >= lines.size()) break;

                        std::string result = parseInstruction(lines[index].second, lines[index].first);
                        localBuffer.push_back(std::move(result));

                        if (localBuffer.size() >= 100) {  // Batch size
                            std::string batchedOutput;
                            for (const auto& str : localBuffer) {
                                batchedOutput += str;
                            }
                            bufferOutput(std::move(batchedOutput));
                            localBuffer.clear();
                        }
                    }
                    if (!localBuffer.empty()) {
                        std::string batchedOutput;
                        for (const auto& str : localBuffer) {
                            batchedOutput += str;
                        }
                        bufferOutput(std::move(batchedOutput));
                    }
                    activeThreads.fetch_sub(1, std::memory_order_relaxed);
                }
                catch (const std::exception& e) {
                    log(LogLevel::EXCEPTION, std::format("Worker thread exception: {}", e.what()));
                    activeThreads.fetch_sub(1, std::memory_order_relaxed);
                }
                };

            std::vector<std::thread> threads;
            for (unsigned int i = 0; i < threadCount; ++i) {
                threads.emplace_back(worker);
            }

            for (auto& thread : threads) {
                thread.join();
            }

            while (activeThreads.load(std::memory_order_relaxed) > 0) {
                std::this_thread::yield();
            }
        }
        else {
            for (const auto& [lineNumber, line] : lines) {
                bufferOutput(parseInstruction(line, lineNumber));
            }
        }
    }
    catch (const FileError& e) {
        log(LogLevel::CRIT, std::format("File error: {}", e.what()));
        throw;
    }
    catch (const std::exception& e) {
        log(LogLevel::CRIT, std::format("Processing error: {}", e.what()));
        throw;
    }
}
