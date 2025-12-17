#pragma once

#include <mutex>
#include <chrono>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <string_view>
#include <unordered_map>


// Values set here are defaults; can be overridden by CLI args
struct CompilerConfig {
  std::string inputFile;
  std::string inputString;
  std::string outputFile;
  int threadCount = 0;
  int benchmarkIterations = 0;
  int logLevel = 0;
  size_t maxMemoryMB = 100;
  bool useMmap = false;
  bool skipConfirm = false;
  bool decompileMode = false;
};

class Timer {
  private:
  std::chrono::time_point<std::chrono::high_resolution_clock> timeStart,
  timeEnd;
  
  public:
  void timeStamp(std::string_view timePoint = "") {
    if (timePoint == "start") {
      timeStart = std::chrono::high_resolution_clock::now();
    } else {
      timeEnd = std::chrono::high_resolution_clock::now();
    }
  }
  
  [[nodiscard]] std::chrono::duration<double, std::milli> getDuration() {
    return std::chrono::duration<double, std::milli>(timeEnd - timeStart);
  }
};

[[nodiscard]] inline constexpr int
getDataType(std::string_view value) noexcept {
  if (value.empty())
  return 2;
if (value[0] == '@' ||
  (value[0] == '-' && value.size() > 1 && value[1] == '@'))
  return 1; // Memory ID
if (value[0] == '#' ||
  (value[0] == '-' && value.size() > 1 && value[1] == '#'))
  return 0; // Register ID
return 2;   // Regular value
}
  
[[nodiscard]] inline constexpr bool
isNegative(std::string_view value) noexcept {
return !value.empty() && value[0] == '-';
}

[[nodiscard]] std::string_view stripSymbols(std::string_view value) noexcept;

[[nodiscard]] std::vector<std::string_view> split(std::string_view s,char delimiter);

[[nodiscard]] inline std::string_view trimComment(std::string_view line) noexcept {
  size_t commentPos = line.find('/');
  return commentPos != std::string_view::npos ? line.substr(0, commentPos) : line;
}
    
[[nodiscard]] constexpr bool isComment(std::string_view line) noexcept {
  return !line.empty() && line[0] == '/';
}

[[nodiscard]] constexpr std::string_view trim(std::string_view s) noexcept {
  auto isSpace = [](unsigned char c) { return std::isspace(c); };
  auto start = std::find_if_not(s.begin(), s.end(), isSpace);
  auto end = std::find_if_not(s.rbegin(), std::string_view::reverse_iterator(start), isSpace).base();
  return std::string_view(&*start, end - start);
}

class JumpLabels {
public:
void addLabel(std::string_view label, int lineNumber);
[[nodiscard]] int getLineNumber(std::string_view label);
private:
std::unordered_map<std::string, int> labels;
std::mutex labelMutex;
};

// Chapter 7 format-specific encoding functions (53-bit max)
// Format F_RR: [52:46] opcode(7b) [45:40] out(6b) [39:34] src1(6b) [33:28]
// src2(6b) [27:22] opt1(6b) [21:16] opt2(6b) [15:0] padding(16b)
uint64_t formatF_RR(int opcode, uint8_t out, uint8_t src1, uint8_t src2,
uint8_t opt1 = 0, uint8_t opt2 = 0);

// Format F_RI: [52:46] opcode(7b) [45:40] out(6b) [39:8] src1(32b) [7:0]
// padding(8b)
uint64_t formatF_RI(int opcode, uint8_t out, uint32_t immediate);

// Format F_RM: [52:46] opcode(7b) [45:40] dest(6b) [39:34] addrReg(6b) [33:28]
// srcReg(6b) [27:0] padding(28b)
uint64_t formatF_RM(int opcode, uint8_t dest, uint8_t addrReg,
uint8_t srcReg = 0);

// Format F_RW: [52:46] opcode(7b) [45:40] out(6b) [39:34] bldg(6b) [33:28]
// x(6b) [27:22] y(6b) [21:0] padding(22b)
uint64_t formatF_RW(int opcode, uint8_t out, uint8_t bldg, uint8_t x,
uint8_t y);

// Format F_JC: [52:46] opcode(7b) [45:40] cond(6b) [39:34] reg(6b) [33:0]
// target(34b)
uint64_t formatF_JC(int opcode, uint8_t cond, uint8_t reg, uint64_t target);
      