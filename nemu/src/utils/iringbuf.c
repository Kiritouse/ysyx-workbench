#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <common.h>
#include <string.h>
#include <utils.h>
#include <unistd.h>
//TODO:进行ring_buffer的单元测试
static int segcnt = 0;
RingBuffer_T *iring_buffer=NULL;//全局iring_buffer
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
void init_ring_buffer(uint64_t length){
    segcnt++;
    const uint64_t size = minPageSize(length);//获取页面大小的整数倍
    //需要利用文件来创建共享内存，所以需要创建文件描述符
    int shm_fd = -1;
    char seg_name[128] = "";
    while(1){
        sprintf(seg_name,"/ringbuffer-shm-%d-%d",getpid(),segcnt);
        //创建文件和用户空间的共享内存
        shm_fd = shm_open(seg_name,O_RDWR|O_CREAT|O_EXCL,0666); 
        if(shm_fd != -1){
            break;
        }
        printf("shm_open error\n");
        nemu_state.state = NEMU_QUIT;
    }
    if(ftruncate(shm_fd,(off_t)2*size) == -1){ //共享文件的内存扩展为2*size
        printf("ftruncate error\n");
        nemu_state.state = NEMU_QUIT;
    }
    //创建共享内存映射，将用户空间的共享内存映射到内核空间(物理内存)
    //mmap(想要映射的内存的起始地址，映射的内存大小，保护模式，映射的标志，文件描述符，文件偏移)
    unsigned char* first_part =
     mmap(NULL,2*size,PROT_READ|PROT_WRITE,MAP_SHARED,shm_fd,(off_t)0);
    if(first_part==MAP_FAILED){
        printf("mmap error\n");
        nemu_state.state = NEMU_QUIT;
    }

    //创建回环缓冲区，将前半部分再映射到后半部分
    //shm_fd:大小是一个2*size的文件，只将其前半部分size映射到first_part+size，即后半部分
    unsigned char* second_part  
    = mmap((char*)first_part + size,
           size, 
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_FIXED,
            shm_fd,
            (off_t)0);
    if(second_part == MAP_FAILED){
        printf("mmap error\n");
        nemu_state.state = NEMU_QUIT;
    }
      //成功建立了映射，关闭文件描述符，后续只需要用指针即可控制共享内存
    close(shm_fd);
    shm_fd = -1;

    iring_buffer = (RingBuffer_T*)malloc(sizeof(RingBuffer_T)+strlen(seg_name)+1);
    if(iring_buffer == NULL){
        munmap(first_part,2*size);
        printf("malloc error\n");
        nemu_state.state = NEMU_QUIT;
    }
    iring_buffer->base = first_part;
    iring_buffer->size = size;
    iring_buffer->read_offset = 0;
    iring_buffer->write_offset = 0;
    strcpy(iring_buffer->name,seg_name);
    
}
void ring_buffer_write(char*in,uint64_t size){
    unsigned char*w_ptr = get_ring_buffer_write_pointer();//获取当前的写指针
    memcpy(w_ptr,in,size);
    ring_buffer_write_advance(size);//更新写指针

}
void ring_buffer_read(char* out,uint64_t size){
    unsigned char* r_ptr = get_ring_buffer_read_pointer();
    memcpy(out,r_ptr,size);
    ring_buffer_read_advance(size);
}
void ring_buffer_destroy(RingBuffer_T* *rb)
{
	int res;

	res = munmap(rb[0]->base, 2 * rb[0]->size);

	(void) res;

	free(rb[0]);
	rb[0] = NULL;
}



