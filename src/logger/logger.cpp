#include "logger.h"
#include <iostream>
#include <cstdarg>
#include <ctime>
#include <iomanip>
#include <mutex>

static FILE *g_logFile = nullptr;
static std::mutex g_logMutex;

static const char* log_level_str[] = {
    "INFO",
    "DANGER",
    "ERROR",
    "DEBUG"
};

void init_logger() {
	g_logFile = fopen("/tmp/chat_server_rooms.log", "a");
    if (!g_logFile) {
        // Fallback to stderr if file cannot be opened
        g_logFile = stderr;
    }

	setvbuf(g_logFile, nullptr, _IOLBF, 0); // line-buffered
}

void close_logger() {
	if (g_logFile && g_logFile != stderr) {
        fclose(g_logFile);
    }
    g_logFile = nullptr;
}

void chr_log(log_level_t level, const char* fmt, ...) {

#ifndef DEBUG
    if (level == LOG_DEBUG) return;
#endif

    if (level < LOG_INFO || level > LOG_DEBUG)
        return;

	std::lock_guard<std::mutex> lock(g_logMutex);
    if (!g_logFile) {
        return;
    }

	time_t now = time(nullptr);
    struct tm tm_buf;
    localtime_r(&now, &tm_buf);

    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &tm_buf);
    fprintf(g_logFile, "[%s] [%s] ", timebuf, log_level_str[level]);

    va_list args;
    va_start(args, fmt);
    vfprintf(g_logFile, fmt, args);
    va_end(args);

    fprintf(g_logFile, "\n");
}

