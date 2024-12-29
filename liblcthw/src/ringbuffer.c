#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "lcthw/dbg.h"
#include "lcthw/ringbuffer.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

static inline uint64_t minPageSize(size_t length) {
    size_t _size = length;
    const long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize <= 0) {
        printf("pagesize error\n");
        return 0;
    }
    uint64_t page_qty = _size / pagesize;
    if (_size % pagesize) {
        page_qty++;
    }
    if (page_qty == 0) {
        page_qty++;
    }
    return page_qty * ((pagesize > 0) ? (uint64_t)pagesize : 4096UL);
}
RingBuffer *RingBuffer_create(int length)
{

    RingBuffer *buffer = calloc(1, sizeof(RingBuffer));
    buffer->length  = length + 1;
    buffer->start = 0;
    buffer->end = 0;
    buffer->buffer = calloc(buffer->length, 1);

    return buffer;
}
rng_buf_t *rng_buf_create_posix(int length)
{
    const uint64_t size = minPageSize(length);

    //获取页面的整数倍的大小
    int shm_fd =-1;
    int seg_counter = rand();
    char seg_name[4096] = "";
    while(1){
        sprintf(seg_name, "/ringbuffer-%d-%d", (int)getpid(), seg_counter);
        shm_fd = shm_open(seg_name, O_RDWR | O_CREAT | O_EXCL, 0600);
    // 获取文件描述符指向共享内存文件
        if(shm_fd != -1){
            break;
        }
    printf("open_shm failed\n");
    }

    if(ftruncate(shm_fd,(off_t)2*size)==-1){ //将共享内存文件扩展为size*2;
        goto ret_null;
    }
    unsigned char* first_part =
        mmap(0,2*size,PROT_READ|PROT_WRITE,MAP_SHARED,shm_fd,(off_t)0);
    if(first_part==MAP_FAILED){
        goto ret_null;
    }
    unsigned char* second_part  
         = mmap((char*)first_part + size,
	            size, 
	             PROT_READ | PROT_WRITE,
	             MAP_SHARED | MAP_FIXED,
	             shm_fd,
	             (off_t)0); //映射文件中的起始地址的偏移
    //因为我们第一次映射的时候文件设备中的2*size都映射到了用户空间中的2*size
    //我们将文件设备中的前半部分size映射到用户空间的后半部分size，这样就实现了内存的镜像
    if(second_part==MAP_FAILED){
        goto ret_null;
    }
    close(shm_fd);
    shm_fd = -1;

    // rng_buf_t *rb =calloc(1, sizeof(rng_buf_t));
    rng_buf_t *rb = malloc (sizeof(rng_buf_t) + strlen(seg_name)); //分配一个空间储存结构体，并且在结构体后还要储存一个字符串
    if(rb==NULL){
        munmap(first_part,2*size);
        goto ret_null;
    }
    rb->base = first_part;
	rb->size = size;
	rb->write_offset_bytes = 0;
	rb->read_offset_bytes = 0;
	strcpy(rb->name, seg_name);//拷贝到char name开头的地址，这样就不用单独malloc字符串了
	return rb;

ret_null:
    if (shm_fd != -1) close(shm_fd);
    shm_unlink(seg_name);
    return NULL;
}
void rng_buf_destroy(rng_buf_t* *rb)
{
	int res;

	res = munmap(rb[0]->base, 2 * rb[0]->size);

	(void) res;

	free(rb[0]);
	rb[0] = NULL;
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

// int main(){
//     rng_buf_t * rng_buf = rng_buf_create_posix(20);
//     unsigned char *w_ptr = rng_buf_write_address(rng_buf);
//     memcpy(w_ptr, "1234", 4);//写入四个字节的数据
// 	rng_buf_write_advance(rng_buf, 4);//更新写指针
//     check(rng_buf_len(rng_buf)==4U,"尝试写入4字节数据后长度不等");
//     check(rng_buf_free_bytes(rng_buf)==rng_buf->size - 4,"剩余的字节空间不等于size-4");

//     unsigned char *r_ptr = rng_buf_read_address(rng_buf);//获取读指针的地址
//     check(memcmp(r_ptr, "1234", 4)==0,"读出的数据和写入的数据不相同");
//     rng_buf_read_advance(rng_buf, 4);//更新读指针
//     check(rng_buf_len(rng_buf)==0U,"读出数据后已有数据的减少出现问题");
// 	check(rng_buf_free_bytes(rng_buf)==rng_buf->size,"读出数据后剩余数据的增加出现问题");
    
// error:
//     return -1;
// }
