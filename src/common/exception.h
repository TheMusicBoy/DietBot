#pragma once

#include <string>
#include <exception>
#include <fmt/core.h>

namespace NDietBot {

class TCommonException
    : public std::exception
{
private:
    std::string mes_;

public: 
    TCommonException(const std::string& m);
    
    const char* what() const noexcept override;
};

#define THROW(...) throw NDietBot::TCommonException(fmt::format(__VA_ARGS__))

#define THROW_ERROR(e, ...) THROW("{} {}", e.what(), fmt::format(__VA_ARGS__))

#define THROW_IF(condition, ...) if (condition) THROW(__VA_ARGS__)

#define THROW_UNLESS(condition, ...) THROW_IF(!condition, __VA_ARGS__)

} // namespace NDietBot
