#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>

// Log levels
typedef enum {
    LOG_INFO,
    LOG_DANGER,
    LOG_ERROR,
    LOG_DEBUG
} log_level_t;

// Main logging function
void chr_log(log_level_t level, const char *fmt, ...);

#define LOG_INFO(fmt, ...) chr_log(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) chr_log(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) chr_log(LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOG_DANGER(fmt, ...) chr_log(LOG_DANGER, fmt, ##__VA_ARGS__)

#endif // LOGGER_H

