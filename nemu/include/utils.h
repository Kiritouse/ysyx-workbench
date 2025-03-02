/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <common.h>
// ----------- state -----------

enum { NEMU_RUNNING, NEMU_STOP, NEMU_END, NEMU_ABORT, NEMU_QUIT };

typedef struct {
  int state;
  vaddr_t halt_pc;
  uint32_t halt_ret;
} NEMUState;

typedef struct{
  unsigned char* base;
  uint64_t size;
  uint64_t write_offset; //read buffer[start]
  uint64_t read_offset; //write buffer[end]
  //最多可以使用的buffer为length-1
  //buffer 为空的时候start =end
  //buffer 满的时候 (end+1)%length = start
  char name[1];
}RingBuffer_T;
extern RingBuffer_T *iring_buffer;//全局iring_buffer

void init_ring_buffer(uint64_t length);
void ring_buffer_destroy(RingBuffer_T** ringbuffer);
void ring_buffer_write(char*buffer,uint64_t size);
void ring_buffer_read(char* out,uint64_t size);
static inline unsigned char*
   get_ring_buffer_write_pointer(){
  return iring_buffer->base+iring_buffer->write_offset;
}

static inline void
ring_buffer_write_advance (uint64_t size)
{
	iring_buffer->write_offset += size;
}

static inline unsigned char *
get_ring_buffer_read_pointer ()
{
	if (iring_buffer->read_offset >= iring_buffer->size)
	{
		iring_buffer->read_offset -= iring_buffer->size;
		iring_buffer->write_offset -= iring_buffer->size;
	}
  return iring_buffer->base + iring_buffer->read_offset;
}

static inline void
ring_buffer_read_advance (
                          uint64_t size)
{
	iring_buffer->read_offset += size;
// 如果读指针大于等于缓冲区长度，那些读写指针同时折返回[0, buffer_size]范围内
//这里需要我们之后能再次插入数据，如果不返回的话再继续再>size处进行offset的话就不能再次读出数据
	if (iring_buffer->read_offset >= iring_buffer->size)
	{
		iring_buffer->read_offset -= iring_buffer->size;
		iring_buffer->write_offset -= iring_buffer->size;
	}
}

static inline uint64_t
rng_buf_len (RingBuffer_T *rb)
{
	return rb->write_offset - rb->read_offset;
}

static inline uint64_t
rng_buf_free_bytes (RingBuffer_T*rb)
{
	return rb->size - rng_buf_len (rb);
}

extern NEMUState nemu_state;

// ----------- timer -----------

uint64_t get_time();

// ----------- log -----------

#define ANSI_FG_BLACK   "\33[1;30m"
#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN    "\33[1;36m"
#define ANSI_FG_WHITE   "\33[1;37m"
#define ANSI_BG_BLACK   "\33[1;40m"
#define ANSI_BG_RED     "\33[1;41m"
#define ANSI_BG_GREEN   "\33[1;42m"
#define ANSI_BG_YELLOW  "\33[1;43m"
#define ANSI_BG_BLUE    "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;45m"
#define ANSI_BG_CYAN    "\33[1;46m"
#define ANSI_BG_WHITE   "\33[1;47m"
#define ANSI_NONE       "\33[0m"

#define ANSI_FMT(str, fmt) fmt str ANSI_NONE

#define log_write(...) IFDEF(CONFIG_TARGET_NATIVE_ELF, \
  do { \
    extern FILE* log_fp; \
    extern bool log_enable(); \
    if (log_enable() && log_fp != NULL) { \
      fprintf(log_fp, __VA_ARGS__); \
      fflush(log_fp); \
    } \
  } while (0) \
)

#define _Log(...) \
  do { \
    printf(__VA_ARGS__); \
    log_write(__VA_ARGS__); \
  } while (0)


#endif
