#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcthw/dbg.h"
#include "lcthw/ringbuffer.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
RingBuffer *RingBuffer_create(int length)
{
    RingBuffer *buffer = calloc(1, sizeof(RingBuffer));
    buffer->length  = length + 1;
    buffer->start = 0;
    buffer->end = 0;
    buffer->buffer = calloc(buffer->length, 1);

    return buffer;
}
RingBuffer *RingBuffer_create_posix(int length)
{
    RingBuffer *buffer = calloc(1, sizeof(RingBuffer));
    check_mem(buffer);

    buffer->length = length + 1;
    buffer->start = 0;
    buffer->end = 0;

    int fd = shm_open("/ringbuffer_shm", O_CREAT | O_RDWR, 0600);
    check(fd != -1, "Failed to open shm.");

    int rc = ftruncate(fd, buffer->length);
    check(rc == 0, "Failed to truncate shm.");

    void *map = mmap(NULL, buffer->length * 2, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);
    check(map != MAP_FAILED, "Failed to mmap.");

    // Map second half (mirror)
    void *map2 = mmap(map + buffer->length, buffer->length, PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_FIXED, fd, 0);
    check(map2 != MAP_FAILED, "Failed to mmap second half.");

    buffer->buffer = map;
    close(fd);

    return buffer;
error:
    if(fd >= 0) close(fd);
    if(buffer) free(buffer);
    return NULL;
}

void RingBuffer_destroy(RingBuffer *buffer)
{
    if(buffer) {
        free(buffer->buffer);
        free(buffer);
    }
}

int RingBuffer_write(RingBuffer *buffer, char *data, int length)
{
    /*
    end == start出现的时候我都要将它们重置为0，使它们从退回缓冲区头部
    */
    if(RingBuffer_available_data(buffer) == 0) { 
        buffer->start = buffer->end = 0;
    }
    
    check(length <= RingBuffer_available_space(buffer),
            "Not enough space: %d request, %d available",
            RingBuffer_available_data(buffer), length);

    void *result = memcpy(RingBuffer_ends_at(buffer), data, length);//因为是char*,所以都是一个字节一个字节索引
    check(result != NULL, "Failed to write data into buffer.");

    RingBuffer_commit_write(buffer, length);//更新end的位置

    return length;
error:
    return -1;
}

int RingBuffer_read(RingBuffer *buffer, char *target,  int amount)
{
    check_debug(amount <= RingBuffer_available_data(buffer),
            "Not enough in the buffer: has %d, needs %d",
            RingBuffer_available_data(buffer), amount);

    void *result = memcpy(target, RingBuffer_starts_at(buffer), amount);
    check(result != NULL, "Failed to write buffer into data.");

    RingBuffer_commit_read(buffer, amount);

    if(buffer->end == buffer->start) {
        buffer->start = buffer->end = 0;
    }

    return amount;
error:
    return -1;
}

bstring RingBuffer_gets(RingBuffer *buffer, int amount)
{
    check(amount > 0, "Need more than 0 for gets, you gave: %d ", amount);
    check_debug(amount <= RingBuffer_available_data(buffer),
            "Not enough in the buffer.");

    bstring result = blk2bstr(RingBuffer_starts_at(buffer), amount);
    check(result != NULL, "Failed to create gets result.");
    check(blength(result) == amount, "Wrong result length.");

    RingBuffer_commit_read(buffer, amount);
    assert(RingBuffer_available_data(buffer) >= 0 && "Error in read commit.");

    return result;
error:
    return NULL;
}
