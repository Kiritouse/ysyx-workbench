#include <common.h>
#ifndef IRINGBUF_H
#define IRINGBUF_H
#define MAX_INST_LEN 128
typedef struct{
    char**buf;
    int front;//起点读指针
    int rear;//末尾写指针
    int size;//

}IRingBuf_T;

void init_iringbuf(int length);
void iringbuf_write(char*inst);
void iringbuf_print();
void iringbuf_free();

#endif