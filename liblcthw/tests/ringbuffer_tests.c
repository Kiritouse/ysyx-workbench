#include "minunit.h"
#include <lcthw/ringbuffer.h>
#include <assert.h>
#include <string.h>
#include <time.h>
clock_t start, end;
double cpu_time_used;
static RingBuffer *ringbuffer = NULL;
char*test_ringbuffer_create(){
    ringbuffer = RingBuffer_create(20);
    mu_assert(ringbuffer != NULL, "Failed to create ringbuffer.");
    return NULL;
}
char*test_ringbuffer_create_posix(){
    ringbuffer = RingBuffer_create_posix(20);
    mu_assert(ringbuffer != NULL, "Failed to create ringbuffer.");
    return NULL;
}
char*test_ringbuffer_write(){
    char *data = "123456";
    int length = strlen(data);
    int rc = RingBuffer_write(ringbuffer, data, length);//返回写入的数据长度
    mu_assert(rc == length, "Failed to write data into ringbuffer.");
    return NULL;
}
char *test_ringbuffer_read() {
    char target[7];
    int rc = RingBuffer_read(ringbuffer, target, 6); // 读取6个字符
    mu_assert(rc == 6, "Failed to read data from ringbuffer.");
    target[6] = '\0'; // 添加字符串结束符
    mu_assert(strcmp(target, "123456") == 0, "Read incorrect data from ringbuffer.");
    return NULL;
}
char *test_ringbuffer_boundary_conditions() {
    // 写入数据直到缓冲区满
    char *data = "abcdefghijklmnopqrst";
    int length = strlen(data);
    int rc = RingBuffer_write(ringbuffer, data, length);
    mu_assert(rc == length, "Failed to write data into ringbuffer.");

    // 读取所有数据
    char target[21];
    rc = RingBuffer_read(ringbuffer, target, length);
    mu_assert(rc == length, "Failed to read data from ringbuffer.");
    target[length] = '\0';
    mu_assert(strcmp(target, "abcdefghijklmnopqrst") == 0, "Read incorrect data from ringbuffer.");

    // 检查缓冲区是否重置
    mu_assert(ringbuffer->start == 0 && ringbuffer->end == 0, "Buffer did not reset correctly.");

    return NULL;
}
char *all_tests() {
    mu_suite_start();
     start = clock();
    mu_run_test(test_ringbuffer_create);
    mu_run_test(test_ringbuffer_write);
    mu_run_test(test_ringbuffer_read);
    mu_run_test(test_ringbuffer_boundary_conditions);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("no posix took %f seconds to execute n", cpu_time_used);
    RingBuffer_destroy(ringbuffer);

    // start = clock();
    // mu_run_test(test_ringbuffer_create_posix);
    // mu_run_test(test_ringbuffer_write);
    // mu_run_test(test_ringbuffer_read);
    // mu_run_test(test_ringbuffer_boundary_conditions);
    // end = clock();
    // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    // printf("posix took %f seconds to execute n", cpu_time_used);

    

    return NULL;
}

RUN_TESTS(all_tests);