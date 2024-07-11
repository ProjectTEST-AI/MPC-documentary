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
#include <bit>
#include "../include/memory_mapped_file.hpp"
#include "../include/exceptions.h"
#include "../include/operations.h"
#include "../include/logging.h"
#include "../include/utils.h"

namespace {
    constexpr size_t BATCH_SIZE = 100;
    thread_local std::vector<std::string> localBuffer;
}

std::string parseInstruction(std::span<const char> line, size_t lineNumber, JumpLabels& jumpLabels) {
    std::string_view lineView(line.data(), line.size());
    std::string_view trimmedLine = trimComment(lineView);
    if (isComment(trimmedLine) || trim(trimmedLine).empty()) {
        return {};
    }

    if (trimmedLine.ends_with(':')) {
        std::string_view label = trim(trimmedLine.substr(0, trimmedLine.size() - 1));
        jumpLabels.addLabel(label, static_cast<int>(lineNumber));
        return {};
    }

    auto operands = split(trimmedLine, ' ');
    if (operands.empty()) {
        return {}; // Empty line after trimming
    }

    std::string_view instructionView = operands[0];

    auto it = Operations::instructionMap.find(instructionView);
    if (it == Operations::instructionMap.end()) {
        std::string upperInstruction(instructionView);
        std::ranges::transform(upperInstruction, upperInstruction.begin(), ::toupper);
        it = Operations::instructionMap.find(upperInstruction);
        if (it == Operations::instructionMap.end()) {
            log(LogLevel::WARN, std::format("Unknown instruction '{}' on line {}, skipping", instructionView, lineNumber));
            return {}; // Skip unknown instructions instead of throwing an exception
        }
        log(LogLevel::WARN, std::format("Instruction '{}' is not capitalized on line {}", instructionView, lineNumber));
    }

    const auto& instructionInfo = it->second;

    std::span<const std::string_view> operandSpan(operands.begin() + 1, operands.end());

    if (!validateOperands(operandSpan, instructionInfo.allowedOperands)) {
        log(LogLevel::WARN, std::format("Invalid operands for instruction '{}' on line {}, skipping", instructionView, lineNumber));
        return {}; // Skip invalid operands instead of throwing an exception
    }

    return instructionInfo.formatFunc(operands) + '\n';
}

void parseAndProcess(std::string_view sourceFile, bool useMultithreading) {
    try {
        MemoryMappedFile mmapFile(sourceFile);
        std::span fileContent = mmapFile.get_span();

        std::vector<std::pair<size_t, std::string_view>> lines;
        lines.reserve(fileContent.size() / 20); // Estimate average line length
        size_t lineNumber = 1;
        const char* begin = fileContent.data();
        const char* end = begin + fileContent.size();
        while (begin < end) {
            const char* newline = std::ranges::find(begin, end, '\n');
            lines.emplace_back(lineNumber++, std::string_view(begin, newline - begin));
            begin = newline + 1;
        }

        JumpLabels jumpLabels;

        if (useMultithreading) {
            unsigned int threadCount = std::thread::hardware_concurrency();
            std::atomic<size_t> lineIndex(0);
            std::vector<std::jthread> threads;
            std::counting_semaphore<> sem(threadCount);

            auto worker = [&]() {
                while (true) {
                    size_t index = lineIndex.fetch_add(1, std::memory_order_relaxed);
                    if (index >= lines.size()) break;

                    sem.acquire();
                    std::string result = parseInstruction(lines[index].second, lines[index].first, jumpLabels);
                    sem.release();

                    if (!result.empty()) {
                        localBuffer.push_back(std::move(result));

                        if (localBuffer.size() >= BATCH_SIZE) {
                            std::string batchedOutput;
                            batchedOutput.reserve(BATCH_SIZE * 20); // Estimate average instruction length
                            for (const auto& str : localBuffer) {
                                batchedOutput += str;
                            }
                            bufferOutput(std::move(batchedOutput));
                            localBuffer.clear();
                        }
                    }
                }
                if (!localBuffer.empty()) {
                    std::string batchedOutput;
                    batchedOutput.reserve(localBuffer.size() * 20);
                    for (const auto& str : localBuffer) {
                        batchedOutput += str;
                    }
                    bufferOutput(std::move(batchedOutput));
                    localBuffer.clear();
                }
                };

            threads.reserve(threadCount);
            for (unsigned int i = 0; i < threadCount; ++i) {
                threads.emplace_back(worker);
            }
        }
        else {
            for (const auto& [lineNumber, line] : lines) {
                std::string result = parseInstruction(line, lineNumber, jumpLabels);
                if (!result.empty()) {
                    bufferOutput(std::move(result));
                }
            }
        }
    }
    catch (const std::exception& e) {
        log(LogLevel::CRIT, std::format("Processing error: {}", e.what()));
        throw;
    }
}
