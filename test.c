#define _GNU_SOURCE
#include "src/log.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_FILE_PATH "./test.txt"

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

void test_file_write()
{
    log_file_open("./test.txt");
    log_fatal("FATAL");
    log_trace("TRACE");
    log_file_close();
    log_fatal("FATAL");

    int res = count_lines(TEST_FILE_PATH);
    remove(TEST_FILE_PATH);
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

void concurrency_log_test()
{
    log_file_open(TEST_FILE_PATH);
    pthread_t thread1, thread2;
    int num0 = 0, num1 = 1;

    pthread_create(&thread1, NULL, writer, &num0);
    pthread_create(&thread2, NULL, writer, &num1);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    log_file_close();
    int res = count_lines(TEST_FILE_PATH);
    remove(TEST_FILE_PATH);
    assert(res == WRITER_REPEATS * 2);
}

void test_open_close()
{
    log_file_open(TEST_FILE_PATH);
    log_file_close();
}

void test_double_open()
{
    log_file_open(TEST_FILE_PATH);
    assert(log_file_open(TEST_FILE_PATH) != 0);
    log_file_close();
}

void test_double_close()
{
    log_file_open(TEST_FILE_PATH);
    log_file_close(TEST_FILE_PATH);
    assert(log_file_close() != 0);
}

int main()
{
    log_level_set(LOG_LEVEL_TRACE);
    log_trace("TRACE");
    log_debug("DEBUG");
    log_info("INFO");
    log_warn("WARN");
    log_error("ERR");
    log_fatal("FATAL");

    test_open_close();
    test_file_write();
    concurrency_log_test();
    test_double_open();
    test_double_close();

    log_info("All tests are passed");
}
