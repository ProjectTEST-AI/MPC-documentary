#include <map>
#include <mutex>
#include <queue>
#include <atomic>
#include <format>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>
#include <condition_variable>
#include "../include/logging.h"
#include "../include/compiler.h"
#include "../include/decompiler.h"
#include "../include/multithreading.h"

namespace {
    std::atomic<size_t> currentMemoryUsage{0};
    constexpr size_t BYTES_PER_MB = 1024 * 1024;

    class MemoryLimiter {
    public:
        explicit MemoryLimiter(size_t maxMB) : maxBytes(maxMB * BYTES_PER_MB) {}

        void allocate(size_t bytes) const {
            currentMemoryUsage += bytes;
        }

        void deallocate(size_t bytes) const {
            if (currentMemoryUsage >= bytes) currentMemoryUsage -= bytes;
            else currentMemoryUsage = 0;
        }

        bool isOverLimit() const {
            return currentMemoryUsage > maxBytes;
        }

        size_t getUsage() const { return currentMemoryUsage; }

    private:
        size_t maxBytes;
    };

    struct Job {
        size_t lineNum;
        std::string content;
    };

    // Generic multi-threaded processor that takes a line processing function
    template<typename ProcessFunc>
    void processMultiThreadGeneric(const CompilerConfig& config, LineSource& source, ProcessFunc processLine) {
        MemoryLimiter limiter(config.maxMemoryMB);

        std::queue<Job> jobQueue;
        std::mutex jobMutex;
        std::condition_variable jobCv;
        bool producerDone = false;

        // Output Reorder Buffer
        std::map<size_t, std::string> reorderBuffer;
        std::mutex resultMutex;
        std::condition_variable resultCv;
        size_t nextExpectedLine = 1;

        // Workers
        unsigned int threadCount = (config.threadCount > 0) ? config.threadCount : std::thread::hardware_concurrency();
        std::vector<std::jthread> workers;

        for (unsigned int i = 0; i < threadCount; ++i) {
            workers.emplace_back([&] {
                while (true) {
                    Job job;
                    {
                        std::unique_lock lock(jobMutex);
                        jobCv.wait(lock, [&] { return !jobQueue.empty() || producerDone; });
                        if (jobQueue.empty() && producerDone) return;
                        job = std::move(jobQueue.front());
                        jobQueue.pop();
                    }

                    // Process line using the provided function
                    std::string res = processLine(job.content, job.lineNum);
                    
                    if (!res.empty()) {
                        limiter.allocate(res.size());
                    }

                    {
                        std::lock_guard lock(resultMutex);
                        reorderBuffer[job.lineNum] = std::move(res);
                    }
                    resultCv.notify_one(); 
                }
            });
        }

        // Reader thread
        std::atomic<bool> readerPaused{false};

        std::jthread reader([&] {
            size_t lineIdx = 0;
            while (true) {
                if (limiter.isOverLimit()) {
                    readerPaused = true;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue; 
                }
                readerPaused = false;
                
                // Batch read from source
                std::vector<Job> batch;
                batch.reserve(50);
                for(int k=0; k<50; ++k) {
                    auto line = source.next();
                    if (!line) break;
                    batch.emplace_back(++lineIdx, std::move(*line));
                }

                if (batch.empty()) break; // EOF

                {
                    std::lock_guard lock(jobMutex);
                    for(auto& job : batch) {
                        jobQueue.push(std::move(job));
                    }
                }
                jobCv.notify_all();
            }
            {
                std::lock_guard lock(jobMutex);
                producerDone = true;
            }
            jobCv.notify_all();
        });

        // Main Thread: Consumer
        auto lastProgressTime = std::chrono::steady_clock::now();
        
        while (true) {
            std::unique_lock lock(resultMutex);
            bool available = resultCv.wait_for(lock, std::chrono::milliseconds(100), [&]{
                return reorderBuffer.contains(nextExpectedLine);
            });

            if (available) {
                std::string val = std::move(reorderBuffer[nextExpectedLine]);
                reorderBuffer.erase(nextExpectedLine);
                lock.unlock();

                if (!val.empty()) {
                    bufferOutput(std::move(val));
                    limiter.deallocate(val.size());
                }
                nextExpectedLine++;
                lastProgressTime = std::chrono::steady_clock::now();
            } else {
                if (producerDone && jobQueue.empty() && reorderBuffer.empty()) break; 
                
                // Timeout logic
                auto now = std::chrono::steady_clock::now();
                if (readerPaused && (now - lastProgressTime > std::chrono::seconds(5))) {
                    if (!config.skipConfirm) {
                        std::cerr << "\n[WARN] Memory Limit Timeout! Stalled for 5s.\n";
                        std::cerr << "Attempts to auto-fix? (y/n): ";
                        char c; 
                        if (std::cin >> c && (c == 'y' || c == 'Y')) {
                            limiter.deallocate(limiter.getUsage()); 
                        } else {
                            throw std::runtime_error("Processing aborted by user due to timeout.");
                        }
                    } else {
                        limiter.deallocate(limiter.getUsage());
                    }
                    lastProgressTime = std::chrono::steady_clock::now();
                }
            }
        }
    }
}

std::unique_ptr<LineSource> createLineSource(const CompilerConfig& config) {
    if (!config.inputString.empty()) {
        return std::make_unique<StringSource>(config.inputString);
    } else if (config.useMmap) {
        return std::make_unique<MmapSource>(config.inputFile);
    } else {
        return std::make_unique<FileSource>(config.inputFile);
    }
}

void compileMultiThread(const CompilerConfig& config, LineSource& source) {
    JumpLabels jumpLabels;
    processMultiThreadGeneric(config, source, [&jumpLabels](const std::string& content, size_t lineNum) {
        return parseInstruction(content, lineNum, jumpLabels);
    });
}

void decompileMultiThread(const CompilerConfig& config, LineSource& source) {
    processMultiThreadGeneric(config, source, [](const std::string& content, size_t lineNum) {
        std::string_view trimmedLine = trim(std::string_view(content));
        if (trimmedLine.empty()) {
            return std::string{};
        }
        try {
            uint64_t encoded = std::stoull(std::string(trimmedLine));
            return decompileInstruction(encoded) + '\n';
        } catch (const std::exception&) {
            log.warn(std::format("Invalid encoded value '{}' on line {}, skipping", trimmedLine, lineNum));
            return std::string{};
        }
    });
}
