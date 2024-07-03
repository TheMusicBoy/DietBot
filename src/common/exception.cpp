#include "exception.h"

namespace NDietBot {

TCommonException::TCommonException(const std::string& m)
    : mes_(m) {}

const char* TCommonException::what() const noexcept {
    return mes_.c_str();
}

} // namespace NDietBot
