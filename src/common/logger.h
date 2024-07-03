#pragma once

#include <string>

#define FMT_HEADER_ONLY

#include <fmt/core.h>

namespace NDietBot {

std::string LogFormat(const std::string& name, const std::string& level, const std::string& m); 

class TLogger {
private:
    std::string name_;

public:
    TLogger(const std::string& name);

    void Write(const std::string& m, const std::string& level);
};

using TLoggerPtr = std::shared_ptr<TLogger>;

////////////////////////////////////////////////////////////////////////////////

#define LOG_MESSAGE(level, message) Logger->Write(message, level)

#define LOG_FORMAT_MESSAGE(level, ...) LOG_MESSAGE(level, fmt::format(__VA_ARGS__))

#define LOG_INFO(...) LOG_FORMAT_MESSAGE("INFO", __VA_ARGS__)

#define LOG_DEBUG(...) LOG_FORMAT_MESSAGE("DEBUG", __VA_ARGS__)

#define LOG_WARNING(...) LOG_FORMAT_MESSAGE("WARNING", __VA_ARGS__)

#define LOG_ERROR(...) LOG_FORMAT_MESSAGE("ERROR", __VA_ARGS__)

#define ELOG_INFO(e, ...) LOG_FORMAT_MESSAGE("INFO", "{} {}", e.what(), fmt::format(__VA_ARGS__))

#define ELOG_DEBUG(e, ...) LOG_FORMAT_MESSAGE("DEBUG", "{} {}", e.what(), fmt::format(__VA_ARGS__))

#define ELOG_WARNING(e, ...) LOG_FORMAT_MESSAGE("WARNING", "{} {}", e.what(), fmt::format(__VA_ARGS__))

#define ELOG_ERROR(e, ...) LOG_FORMAT_MESSAGE("ERROR", "{} {}", e.what(), fmt::format(__VA_ARGS__))

} // namespace NDietBot
