#include "logger.h"
#include <iostream>
#include <cstdarg>
#include <ctime>
#include <iomanip>

static const char* log_level_str[] = {
    "INFO",
    "DANGER",
    "ERROR",
    "DEBUG"
};

void chr_log(log_level_t level, const char* fmt, ...) {

#ifndef DEBUG
    if (level == LOG_DEBUG) return;
#endif

    if (level < LOG_INFO || level > LOG_DEBUG)
        return;

    // Get current time
    std::time_t t = std::time(nullptr);
    std::tm tm_info;
#ifdef _WIN32
    localtime_s(&tm_info, &t);
#else
    localtime_r(&t, &tm_info);
#endif

    std::cout << "[" << std::put_time(&tm_info, "%Y-%m-%d %H:%M:%S") << "] "
              << log_level_str[level] << ": ";

    // Print formatted message
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    std::cout << buffer << std::endl;
}

