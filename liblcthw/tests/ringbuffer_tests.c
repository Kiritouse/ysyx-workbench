#include "minunit.h"
#include <lcthw/ringbuffer.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
clock_t start, end;
double cpu_time_used;
static RingBuffer *ringbuffer = NULL;
static rng_buf_t*rng_buf_posix = NULL;
char*test_ringbuffer_create(){
    ringbuffer = RingBuffer_create(40);
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
char* test_ringbuffer_create_posix(){
    rng_buf_posix = rng_buf_create_posix(40);
    mu_assert(rng_buf_posix != NULL, "Failed to create ringbuffer -- posix.");
    return NULL;
}
char* test_ringbuffer_write_posix(){
    unsigned char* w_ptr = rng_buf_write_address(rng_buf_posix);
    memcpy(w_ptr,"123456",6);
    rng_buf_write_advance(rng_buf_posix,6);
    mu_assert(rng_buf_len(rng_buf_posix)==6U,"Failed to write data into ringbuffer -- posix.")
    mu_assert(rng_buf_free_bytes(rng_buf_posix)==rng_buf_posix->size-6,"The number of remaining bytes is inccorect");
    return NULL;
}
char* test_ringbuffer_read_posix(){
    unsigned char* r_ptr = rng_buf_read_address(rng_buf_posix);
    mu_assert(memcmp(r_ptr,"123456",6)==0,"Failed to read date from ringbuffer --posix");
    rng_buf_read_advance(rng_buf_posix,6);
    mu_assert(rng_buf_len(rng_buf_posix)==0U,"The number of remaining bytes is inccorect after reading -- posix");
    mu_assert(rng_buf_free_bytes(rng_buf_posix)==rng_buf_posix->size,"The number of remaining bytes is inccorect after reading -- posix");

    return NULL;
}
char* perform_random_op(){
    rng_buf_t *_rng_buf = rng_buf_create_posix(40);
    RingBuffer* _ringbuffer = RingBuffer_create(_rng_buf->size);//保证字节数一致
    //生成随机数据，写入两个buffer
    unsigned char rand_data[_rng_buf->size-1];
    for(unsigned i = 0;i<sizeof(rand_data);i++){
        rand_data[i] = random();
    }
    start = clock();
    unsigned char* w_ptr = rng_buf_write_address(_rng_buf);
    memcpy(w_ptr,rand_data,sizeof(rand_data));
    rng_buf_write_advance(_rng_buf,sizeof(rand_data));

    unsigned char* r_ptr = rng_buf_read_address(_rng_buf);
    rng_buf_read_advance(_rng_buf,sizeof(rand_data));
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("posix took %f seconds to execute --test \n", cpu_time_used);

    start = clock();
    int rc = RingBuffer_write(_ringbuffer,rand_data,sizeof(rand_data));
    char target[sizeof(rand_data)];
    rc = RingBuffer_read(_ringbuffer,target,sizeof(rand_data));
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("no posix took %f seconds to execute -- test \n", cpu_time_used);
    RingBuffer_destroy(_ringbuffer);
    rng_buf_destroy(_rng_buf);



}
char *all_tests() {
    mu_suite_start();
    mu_run_test(test_ringbuffer_create);
   //  start = clock();
    mu_run_test(test_ringbuffer_write);
    mu_run_test(test_ringbuffer_read);
   //mu_run_test(test_ringbuffer_boundary_conditions);
    //end = clock();
    //cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    //printf("no posix took %f seconds to execute \n", cpu_time_used);
    RingBuffer_destroy(ringbuffer);

    mu_run_test(test_ringbuffer_create_posix);
    // //start = clock();
    mu_run_test(test_ringbuffer_write_posix);
    mu_run_test(test_ringbuffer_read_posix);
    // //end = clock();
    // //cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    // //printf("posix took %f seconds to execute \n", cpu_time_used);
    rng_buf_destroy(rng_buf_posix);

    mu_run_test(perform_random_op);


    return NULL;
}

RUN_TESTS(all_tests);