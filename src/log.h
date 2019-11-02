/**
 * Copyright (c) 2017 rxi
 * Copyright (c) 2019 Anatolii Kurotych
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#define LOG_VERSION "0.1.2"

enum
{
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
};

#define log_trace(...) log_log(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_log(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__); abort();
void log_log(int level, const char* file, int line, const char* fmt, ...);

// open file stream
int log_file_open(const char* file_path);
int log_file_close();

void log_level_set(int level);
void log_quiet_set(bool enable);

// Multithreaded mode is enabled by default
// Set false only if your program is single threaded.
void log_multithreading(bool enable);

#endif
