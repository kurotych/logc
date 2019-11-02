#define _GNU_SOURCE
#include "src/log.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int count_lines(const char* file_path)
{
    int lines = 0;
    FILE* fp = fopen(file_path, "r");
    char ch;
    while (!feof(fp))
    {
        ch = fgetc(fp);
        if (ch == '\n')
        {
            lines++;
        }
    }
    fclose(fp);
    return lines;
}

void file_write_test()
{
    const char* file_path = "./test.txt";
    log_file_open("./test.txt");
    log_fatal("FATAL");
    log_trace("TRACE");
    log_file_close();
    log_fatal("FATAL");

    int res = count_lines(file_path);
    remove(file_path);
    assert(res == 2);
}

#define WRITER_REPEATS 2000
void* writer(void* arg)
{
    int* n = (int*)arg;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(*n, &cpuset);

    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    for (int i = 0; i < WRITER_REPEATS; i++)
        log_debug("DUMMY log");
    return NULL;
}

void concurrent_log_test()
{
    const char* file_path = "./test.txt";
    log_file_open(file_path);
    pthread_t thread1, thread2;
    int num0 = 0, num1 = 1;

    pthread_create(&thread1, NULL, writer, &num0);
    pthread_create(&thread2, NULL, writer, &num1);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    log_file_close();
    int res = count_lines(file_path);
    remove(file_path);
    assert(res == WRITER_REPEATS * 2);
}

void test_open_close()
{
    const char* file_path = "./test.txt";
    log_file_open(file_path);
    log_file_close();
}
int main()
{
    log_level_set(LOG_TRACE);
    log_trace("TRACE");
    log_debug("DEBUG");
    log_info("INFO");
    log_warn("WARN");
    log_error("ERR");
    log_fatal("FATAL");

    test_open_close();
    file_write_test();
    concurrent_log_test();

    log_info("All tests are passed");
}
