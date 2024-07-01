#include "logger.h"

#include <iostream>
#include <fmt/chrono.h>
#include <chrono>

namespace NDietBot {

std::string LogFormat(const std::string& name, const std::string& level, const std::string& m) {
    auto time = std::chrono::system_clock::now();
    return fmt::format("{:%F %T}\t[{}]\t{}\n", time, level, m);
}

TLogger::TLogger(const std::string& name)
    : name_(name) {}

void TLogger::Write(const std::string& m, const std::string& level) {
    std::cout << LogFormat(name_, level, m) << std::flush;
}

} // namespace NDietBot
