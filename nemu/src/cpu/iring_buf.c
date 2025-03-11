#include <cpu/iring_buf.h>

IRingBuf_T *global_iring_buf = NULL;
void init_iringbuf(int length){
    global_iring_buf = calloc(1,sizeof(IRingBuf_T));
    global_iring_buf->buf =  (char **)malloc( sizeof(char *) * (length + 1) );
    for(int i = 0;i < length + 1 ; i++)
        global_iring_buf -> buf[i] = malloc( sizeof(char) * MAX_INST_LEN);
    
    global_iring_buf->front = 0;
    global_iring_buf->rear = 0; //指向末尾元素的下一个元素
    global_iring_buf->size = length + 1;//留出一个空位防止出现start=end的情况
}

void iringbuf_write(char* inst){
    strncpy(global_iring_buf->buf[global_iring_buf->rear],inst,MAX_INST_LEN);
    global_iring_buf->buf[global_iring_buf->rear][MAX_INST_LEN - 1] = '\0';
    global_iring_buf->rear = (global_iring_buf->rear + 1) % global_iring_buf->size;

    if((global_iring_buf->rear+1)%(global_iring_buf->size) == global_iring_buf->front){
        //为了防止出现start=end的情况，所以要留出一个空位，所以不能用rear++ == front，不然无法判断为空的情况
        global_iring_buf->front = (global_iring_buf->front + 1) % global_iring_buf->size;
    }
}

void iringbuf_print(){
    int empty =(global_iring_buf->rear)%(global_iring_buf->size); //防止越界
    for(int i = global_iring_buf->front; i != empty; i = (i + 1) % global_iring_buf->size){
        if((i + 1) % (global_iring_buf-> size) == empty)
            printf("-->  %s\n",global_iring_buf -> buf[i]);
        else
            printf("     %s\n",global_iring_buf -> buf[i]);
    }
}

void iringbuf_free(){
    if(global_iring_buf == NULL)
        return;
    
    for(int i = 0;i<global_iring_buf->size;i++){
        free(global_iring_buf->buf[i]);
    }

    free(global_iring_buf->buf);
    free(global_iring_buf);
    global_iring_buf = NULL;
}

