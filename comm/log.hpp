#pragma once
#include "util.hpp"
#include <iostream>
namespace core_log
{
using namespace core_util;
enum level
{
    debug,
    info,
    warning,
    error,
    fatal
};
inline std::ostream& log(const char* level, const char* filename, int line)
{
    std::string message;
    message += "[";
    message += level;
    message += "]";

    message += "[";
    message += filename;
    message += "]";

    message += "[";
    message += std::to_string(line);
    message += "]";

    message += "[";
    message += TimeUtil::GetTimeStamp();
    message += "]";

    std::cout << message;

    return std::cout;
}
#define Log(level) log(#level, __FILE__, __LINE__)

} // namespace core_log
