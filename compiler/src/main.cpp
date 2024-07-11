#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <ranges>
#include <format>
#include <future>
#include <algorithm>
#include <filesystem>
#include <execution>
#include "../include/utils.h"
#include "../include/logging.h"
#include "../include/operations.h"
#include "../include/exceptions.h"

namespace { Timer timer; std::vector<double> durations; };
void parseAndProcess(std::string_view sourceFile, bool useMultithreading);

void pauseExit(const int code) {
    try {
        flushOutput();
        flushLogs();
        timer.timeStamp();
        auto duration = timer.getDuration();

        if (!durations.empty()) {
            auto [min, max] = std::minmax_element(durations.begin(), durations.end());
            double average = std::accumulate(durations.begin(), durations.end(), 0.0) / durations.size();

            std::cout << std::format("\nBenchmark results ({} iterations):\n"
                "Average time: {:.2f} ms\n"
                "Min time    : {:.2f} ms\n"
                "Max time    : {:.2f} ms\n",
                durations.size(), average, *min, *max);
        }

        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
        duration -= minutes;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        duration -= seconds;
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        duration -= milliseconds;
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);

        std::cout << "\nIn total, Transpiler took ";
        if (minutes.count() > 0) {
            std::cout << minutes.count() << " minutes, ";
        }
        if (seconds.count() > 0 || minutes.count() > 0) {
            std::cout << seconds.count() << " seconds, ";
        }
        if (milliseconds.count() > 0 || seconds.count() > 0 || minutes.count() > 0) {
            std::cout << milliseconds.count() << " milliseconds, and ";
        }
        std::cout << microseconds.count() << " microseconds.\n";

        std::cout << "Exiting with code " << code << " || ";
        std::system("pause");
    }
    catch (const std::exception& e) {
        log(LogLevel::CRIT, std::format("Error in pauseExit: {}", e.what()));
    }

    std::exit(code);
}

void runBenchmark(std::string_view sourceFile, bool multithread, int iterations) {
    Timer benchmarkTime;
    durations.reserve(iterations);
    std::cout << "-- BENCHMARKING IN PROGRESS --\n";

    for (int i = 1; i <= iterations; ++i) {
        try {
            resetOutputBuffer();
            benchmarkTime.timeStamp("start");
            parseAndProcess(sourceFile, multithread);
            benchmarkTime.timeStamp("end");
            durations.push_back(benchmarkTime.getDuration().count());
        }
        catch (const std::exception& e) {
            log(LogLevel::EXCEPTION, std::format("Error during benchmark iteration {}: {}", i, e.what()));
        }
    }
}

//! Benchmark can only be enabled in console when executing program with --bm (iterations)
int main(int argc, char* argv[]) {
    int logLevel;
    bool multithread;
    int benchmarkIterations = 10000;
    std::string sourceFile, outputFile;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "--bm" && i + 1 < argc) {
            benchmarkIterations = std::stoi(argv[++i]);
        }
    }

    // Benchmark check
    if (benchmarkIterations != 0) {
        std::cout << std::format("-- BENCHMARK {}x ACTIVE --\n", benchmarkIterations);
    }
    else if (benchmarkIterations < 0) {
        std::cout << "-- ERR: BENCHMARK ITERATION CANNOT BE LESS THAN 0 --\nPROCEEDING..\n";
    }
    else {
        std::cout << "-- MACl to Mach-C transpiler --\n";
    }

    std::cout << "File Name: ";
    std::getline(std::cin, sourceFile);

    std::cout << "Output File Name (leave blank for console output only): ";
    std::getline(std::cin, outputFile);

    std::cout << "Logging Level (0-3): ";
    std::cin >> logLevel;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    setLogLevel(std::clamp(logLevel, 0, 3));

    std::cout << "Use Multithreading? (y/n): ";
    char multiThreadChoice;
    std::cin >> multiThreadChoice;
    multithread = (multiThreadChoice == 'y' || multiThreadChoice == 'Y');

    if (!std::filesystem::exists(sourceFile)) {
        log(LogLevel::CRIT, std::format("Source file '{}' does not exist.", sourceFile));
        pauseExit(1);
    }

    if (!outputFile.empty()) {
        try {
            setOutputFile(outputFile);
            log(LogLevel::LOW, std::format("Output file set to: {}", outputFile));
        }
        catch (const std::exception& e) {
            log(LogLevel::CRIT, std::format("Failed to set output file: {}", e.what()));
            setOutputFile("");
        }
    }

    try {
        timer.timeStamp("start");
        if (benchmarkIterations > 0) {
            runBenchmark(sourceFile, multithread, benchmarkIterations);
        }
        else {
            parseAndProcess(sourceFile, multithread);
            log(LogLevel::HIGH, "Processing completed successfully.\n");
        }
        flushLogs();
        pauseExit(0);
    }
    catch (const InvalidSyntax& e) {
        log(LogLevel::EXCEPTION, e.what());
        pauseExit(1);
    }
    catch (const FileError& e) {
        log(LogLevel::EXCEPTION, e.what());
        pauseExit(2);
    }
    catch (const std::exception& e) {
        log(LogLevel::EXCEPTION, std::string("Unexpected error: ") + e.what());
        pauseExit(99);
    }
}
