#include <string>
#include <vector>
#include <thread>
#include <format>
#include <numeric>
#include <iostream>
#include <optional>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include "../include/utils.h"
#include "../include/logging.h"
#include "../include/compiler.h"
#include "../include/decompiler.h"
#include "../include/multithreading.h"

namespace {
    Timer timer;
    std::vector<double> durations;

    // Forward declaration
    [[noreturn]] void flushAndExit(int code);

    void printHelp() {
        std::cout << "Mach-C Compiler CLI\n\n"
                << " ./machc [options]\n\n"
                << "Usage:\n"
                << "  -i, --input <file>       Input file path\n"
                << "  -s, --string <code>      Input code as string\n"
                << "  -o, --output <file>      Output file path\n"
                << "  -d, --decompile          Decompile mode\n"
                << "  -n <threads>             Thread count (0=auto)\n"
                << "  --benchmark-mode <iter>  Run benchmark iterations\n"
                << "  -l, --log-level <0-3>    Set log level\n"
                << "  --max-memory <MB>        Max memory limit\n"
                << "  --map-file-to-memory     Use memory mapping (fast)\n"
                << "  -y                       Skip any confirmation(s)\n"
                << "  -h, --help               Shows this help message\n";
    }

    CompilerConfig parseCLI(int argc, char* argv[]) {
        CompilerConfig config;
        enum class ArgOption {
            Input, String, Output, Threads, Benchmark, LogLevel, MaxMemory, MapFile, SkipConfirm, Decompile, Help, Unknown
        };

        static const std::unordered_map<std::string, ArgOption> optionMap = {
            {"-i", ArgOption::Input}, {"--input", ArgOption::Input},
            {"-s", ArgOption::String}, {"--string", ArgOption::String},
            {"-o", ArgOption::Output}, {"--output", ArgOption::Output},
            {"-d", ArgOption::Decompile}, {"--decompile", ArgOption::Decompile},
            {"-n", ArgOption::Threads},
            {"--benchmark-mode", ArgOption::Benchmark},
            {"-l", ArgOption::LogLevel}, {"--log-level", ArgOption::LogLevel},
            {"--max-memory", ArgOption::MaxMemory},
            {"--map-file-to-memory", ArgOption::MapFile},
            {"-y", ArgOption::SkipConfirm},
            {"-h", ArgOption::Help}, {"--help", ArgOption::Help}
        };

        std::vector<std::string_view> args(argv, argv + argc);
        size_t i = 1;
        while (i < args.size()) {
            std::string arg(args[i++]);
            auto it = optionMap.find(arg);
            ArgOption option = (it != optionMap.end()) ? it->second : ArgOption::Unknown;

            auto requireValue = [&](const std::string& flag) -> std::optional<std::string_view> {
                if (i < args.size()) {
                    return args[i++];
                }
                log.warn(std::format("Missing value for argument: {}", flag));
                return std::nullopt;
            };

            switch (option) {
                case ArgOption::Input: {
                    if (auto val = requireValue(arg)) config.inputFile = *val;
                    break;
                }
                case ArgOption::String: {
                    if (auto val = requireValue(arg)) config.inputString = *val;
                    break;
                }
                case ArgOption::Output: {
                    if (auto val = requireValue(arg)) config.outputFile = *val;
                    break;
                }
                case ArgOption::Threads: {
                    if (auto val = requireValue(arg)) config.threadCount = std::stoi(std::string(*val));
                    break;
                }
                case ArgOption::Benchmark: {
                    if (auto val = requireValue(arg)) config.benchmarkIterations = std::stoi(std::string(*val));
                    break;
                }
                case ArgOption::LogLevel: {
                    if (auto val = requireValue(arg)) config.logLevel = std::clamp(std::stoi(std::string(*val)), 0, 3);
                    break;
                }
                case ArgOption::MaxMemory: {
                    if (auto val = requireValue(arg)) config.maxMemoryMB = std::stoul(std::string(*val));
                    break;
                }
                case ArgOption::MapFile:
                    config.useMmap = true;
                    break;
                case ArgOption::Decompile:
                    config.decompileMode = true;
                    break;
                case ArgOption::SkipConfirm:
                    config.skipConfirm = true;
                    break;
                case ArgOption::Help:
                    printHelp();
                    std::exit(0);
                case ArgOption::Unknown:
                default:
                    log.warn(std::format("Unknown argument: {}", arg));
                    break;
            }
        }
        return config;
    }

    void interactiveInput(CompilerConfig& config) {
        // If skipConfirm is set, don't prompt - use whatever values are already set
        if (config.skipConfirm) {
            if (config.threadCount == 0) {
                config.threadCount = 1; // Default to single thread
            }
            return;
        }
        
        if (config.inputFile.empty() && config.inputString.empty()) {
            std::cout << "Input File Name (or leave empty for string input): ";
            std::getline(std::cin, config.inputFile);
            if (config.inputFile.empty()) {
                std::cout << "Enter Input String: ";
                std::getline(std::cin, config.inputString);
            }
        }

        if (config.outputFile.empty()) {
            std::cout << "Output File Name (leave blank for console): ";
            std::getline(std::cin, config.outputFile);
        }

        if (config.threadCount == 0) { // If not set by flag (assuming 0 is default/unset)
             std::cout << "Use Multithreading? (y/n, default n): ";
             std::string choice;
             std::getline(std::cin, choice);
             if (choice == "y" || choice == "Y") {
                 config.threadCount = std::thread::hardware_concurrency(); 
                 std::cout << "Set to " << config.threadCount << " threads (Auto).\n";
             } else {
                 config.threadCount = 1; // Explicit single thread
             }
        }
    }

    bool confirmConfig(const CompilerConfig& config) {
        if (config.skipConfirm) return true;

        std::cout << "\n--- Configuration Summary ---\n";
        std::cout << "Input: " << (config.inputFile.empty() ? "[String Input]" : config.inputFile) << "\n";
        std::cout << "Output: " << (config.outputFile.empty() ? "[Console]" : config.outputFile) << "\n";
        std::cout << "Threads: " << config.threadCount << "\n";
        std::cout << "Benchmark: " << config.benchmarkIterations << "\n";
        std::cout << "Log Level: " << config.logLevel << "\n";
        std::cout << "Max Memory: " << config.maxMemoryMB << " MB\n";
        std::cout << "Memory Map: " << (config.useMmap ? "Yes" : "No") << "\n";
        
        if (config.useMmap && !config.inputFile.empty()) {
             try {
                 if (std::filesystem::exists(config.inputFile) && std::filesystem::file_size(config.inputFile) > 1024 * 1024 * 1024) {
                     std::cout << "WARNING: Input file is > 1GB. Memory mapping may consume significant resources.\n";
                 }
             } catch (const std::exception& e) {
                 log.critical(std::format("Error: Failed to check file size : {}", e.what()));
                 flushAndExit(1);
             }
        }

        while (true) {
            std::cout << "Proceed? (y/n): ";
            std::string line;
            std::getline(std::cin, line);
            if (line == "y" || line == "Y") return true;
            if (line == "n" || line == "N") return false;
        }
    }

    [[noreturn]] void flushAndExit(int code) {
         // Flush compiled output first
         flushOutput();
         
         // Flush all log messages
         log.flush();
         
         if (durations.empty() && timer.getDuration().count() > 0) {
             // Single run stats
             auto duration = timer.getDuration();
             std::cout << "\nTotal time: " << duration.count() << " ms\n";
         } else if (!durations.empty()) {
             // Benchmark stats
             auto [min, max] = std::ranges::minmax_element(durations);
             double avg = std::accumulate(durations.begin(), durations.end(), 0.0) / durations.size();
             std::cout << std::format("\nBenchmark ({} runs):\nAvg: {:.2f} ms | Min: {:.2f} ms | Max: {:.2f} ms\n",
                durations.size(), avg, *min, *max);
         }
         std::cout << "Exiting with code " << code << " || ";
         std::exit(code);
    }

    void parseAndProcess(const CompilerConfig& config) {
        auto source = createLineSource(config);
        
        if (config.decompileMode) {
            if (config.threadCount == 1) {
                decompileSingleThread(*source);
            } else {
                decompileMultiThread(config, *source);
            }
        } else {
            if (config.threadCount == 1) {
                compileSingleThread(*source);
            } else {
                compileMultiThread(config, *source);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        auto config = parseCLI(argc, argv);
        
        // Handle interactive input or set defaults
        interactiveInput(config);

        log.setLevel(config.logLevel);

        if (!confirmConfig(config)) {
            std::cout << "Aborted by user.\n";
            return 0;
        }

        if (!config.outputFile.empty()) {
            setOutputFile(config.outputFile);
        }

        if (config.benchmarkIterations > 0) {
            std::cout << "-- BENCHMARK MODE --\n";
            durations.reserve(config.benchmarkIterations);
            for (int i = 0; i < config.benchmarkIterations; ++i) {
                resetOutputBuffer();
                timer.timeStamp("start");
                parseAndProcess(config);
                timer.timeStamp("end");
                durations.push_back(timer.getDuration().count());
            }
        } else {
            timer.timeStamp("start");
            parseAndProcess(config);
            timer.timeStamp("end");
        }
        
        flushAndExit(0);
    } catch (const std::exception& e) {
        log.critical(std::format("Error: {}", e.what()));
        flushAndExit(1);
    }
}
