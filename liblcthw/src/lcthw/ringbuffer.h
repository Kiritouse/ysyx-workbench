#ifndef _lcthw_RingBuffer_h
#define _lcthw_RingBuffer_h
#include <bstring/bstrlib.h>
#include <stdint.h>
typedef struct {
    char *buffer;
    int length;
    int start; //读操作从start开始 read buffer[start]
    int end;//写操作从end开始write buffer[end]
    //最大使用量为length-1，那么为空的时候start=end
    //为full的时候(start+1)%N=end
char name[1];
} RingBuffer;
struct rng_buf {
	unsigned char *base;
	uint64_t size;
	uint64_t write_offset_bytes;
	uint64_t read_offset_bytes;

	char name[1];
};
//posix优化
typedef struct rng_buf rng_buf_t;

RingBuffer *RingBuffer_create(int length);

void RingBuffer_destroy(RingBuffer *buffer);

int RingBuffer_read(RingBuffer *buffer, char *target, int amount);

int RingBuffer_write(RingBuffer *buffer, char *data, int length);


rng_buf_t *rng_buf_create_posix(int length);
void rng_buf_destroy(rng_buf_t* *rb);
static inline unsigned char *
rng_buf_write_address (struct rng_buf *rb)
{
	return rb->base + rb->write_offset_bytes;
}

static inline void
rng_buf_write_advance (struct rng_buf *rb,
                           uint64_t size)
{
	rb->write_offset_bytes += size;
}

static inline unsigned char *
rng_buf_read_address (struct rng_buf *rb)
{
	return rb->base + rb->read_offset_bytes;
	if (rb->read_offset_bytes >= rb->size)
	{
		rb->read_offset_bytes -= rb->size;
		rb->write_offset_bytes -= rb->size;
	}
}

static inline void
rng_buf_read_advance (struct rng_buf *rb,
                          uint64_t size)
{
	rb->read_offset_bytes += size;
// 如果读指针大于等于缓冲区长度，那些读写指针同时折返回[0, buffer_size]范围内
//这里需要我们之后能再次插入数据，如果不返回的话再继续再>size处进行offset的话就不能再次读出数据
	if (rb->read_offset_bytes >= rb->size)
	{
		rb->read_offset_bytes -= rb->size;
		rb->write_offset_bytes -= rb->size;
	}
}

static inline uint64_t
rng_buf_len (struct rng_buf *rb)
{
	return rb->write_offset_bytes - rb->read_offset_bytes;
}

static inline uint64_t
rng_buf_free_bytes (struct rng_buf *rb)
{
	return rb->size - rng_buf_len (rb);
}







int RingBuffer_empty(RingBuffer *buffer);

int RingBuffer_full(RingBuffer *buffer);

int RingBuffer_available_data(RingBuffer *buffer);

int RingBuffer_available_space(RingBuffer *buffer);

bstring RingBuffer_gets(RingBuffer *buffer, int amount);

#define RingBuffer_available_data(B) (((B)->end + 1) % (B)->length - (B)->start - 1)

#define RingBuffer_available_space(B) ((B)->length - (B)->end - 1) //如果end已经循环了一遍了，此时end<length-1，那么此处是小于0的

#define RingBuffer_full(B) (RingBuffer_available_data((B)) - (B)->length == 0)

#define RingBuffer_empty(B) (RingBuffer_available_data((B)) == 0)

#define RingBuffer_puts(B, D) RingBuffer_write((B), bdata((D)), blength((D)))

#define RingBuffer_get_all(B) RingBuffer_gets((B), RingBuffer_available_data((B)))

#define RingBuffer_starts_at(B) ((B)->buffer + (B)->start)

#define RingBuffer_ends_at(B) ((B)->buffer + (B)->end)

#define RingBuffer_commit_read(B, A) ((B)->start = ((B)->start + (A)) % (B)->length)

#define RingBuffer_commit_write(B, A) ((B)->end = ((B)->end + (A)) % (B)->length)

#endif