/*
 * Copyright (c) 2017 rxi
 * Copyright (c) 2019 Anatolii Kurotych
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <pthread.h>
#include <time.h>

#include "log.h"

// Default settings
static FILE* fd = NULL;
static int level = LOG_LEVEL_DEBUG;
static bool quiet = false;
static bool multitreading = true;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static const char* level_names[] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};

#ifndef LOG_NO_USE_COLOR
static const char* level_colors[] = {"\x1b[35m", "\x1b[31m", "\x1b[33m",
                                     "\x1b[32m", "\x1b[36m", "\x1b[94m"};
#endif

int log_file_open(const char* file_path)
{
    pthread_mutex_lock(&mutex);
    if (fd != NULL)
    {
        fprintf(stderr, "The log file is already opened\n");
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    fd = fopen(file_path, "a");
    if (fd == NULL)
    {
        fprintf(stderr, "Can't open log file %s\n", file_path);
        pthread_mutex_unlock(&mutex);
        return -2;
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

int log_file_close()
{
    pthread_mutex_lock(&mutex);
    if (fd == NULL)
    {
        fprintf(stderr, "Log file already closed\n");
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    int ret = fclose(fd);
    fd = NULL;
    pthread_mutex_unlock(&mutex);
    return ret;
}

void log_multithreading(bool enable) { multitreading = enable; }

void log_level_set(int lvl)
{
    if (multitreading) pthread_mutex_lock(&mutex);
    level = lvl;
    if (multitreading) pthread_mutex_unlock(&mutex);
}

void log_quiet_set(bool enable)
{
    if (multitreading) pthread_mutex_lock(&mutex);
    quiet = enable;
    if (multitreading) pthread_mutex_unlock(&mutex);
}

void log_log(int lvl, const char* file, int line, const char* fmt, ...)
{
    if (lvl > level)
    {
        return;
    }

    if (multitreading) pthread_mutex_lock(&mutex);
    /* Get current time */
    time_t t = time(NULL);
    struct tm* lt = localtime(&t);

    /* Log to stderr */
    if (!quiet)
    {
        va_list args;
        char buf[16];
        buf[strftime(buf, sizeof(buf), "%H:%M:%S", lt)] = '\0';
#ifndef LOG_NO_USE_COLOR
        fprintf(stderr, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", buf, level_colors[lvl],
                level_names[lvl], file, line);
#else
        fprintf(stderr, "%s %-5s %s:%d: ", buf, level_names[lvl], file, line);
#endif
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fprintf(stderr, "\n");
        fflush(stderr);
    }

    /* Log to file */
    if (fd)
    {
        va_list args;
        char buf[32];
        buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt)] = '\0';
        fprintf(fd, "%s %-5s %s:%d: ", buf, level_names[lvl], file, line);
        va_start(args, fmt);
        vfprintf(fd, fmt, args);
        va_end(args);
        fprintf(fd, "\n");
        fflush(fd);
    }

    if (multitreading) pthread_mutex_unlock(&mutex);
}
