#include "cycle_buffer.h"

//#include "xil_types.h"

status_buffer_t init_and_clean_buf(cycle_buf_t *buf){
    status_buffer_t result = BUFFER_SUCCESS;

    buf->head = 0;
    buf->tail = 0;

    return result;
}

// be carefull with size!
// using for 1 byte:        push_value(&buf, &one_byte, 1);
// using 10 byte packet:    push_value(&buf, (char *)packet_bytes, 10);
status_buffer_t push_bytes(cycle_buf_t * buf, unsigned char * new_byte, unsigned short size){
    status_buffer_t result = BUFFER_SUCCESS;
    
    if (get_empty_len_buf(buf) >= size){
        if (new_byte != NULL){
            while(size--) {                                 // for (int i = buf->tail; (buf->tail - i) < size; i+=1){
                buf->byte[buf->tail++] = *new_byte++;
                if (buf->tail == BUF_LENGHT) buf->tail = 0;
            }
        }
        else {
            if ((buf->tail + size) >= BUF_LENGHT) buf->tail = (buf->tail + size) - BUF_LENGHT;
            else  buf->tail = (buf->tail + size);
        }
    } else {
        result = BUFFER_FULL;
    }

    return result;
}

// readed bytes will not available in cycle buf! 
status_buffer_t pull_bytes(cycle_buf_t * buf, unsigned char * readed_byte, unsigned short size){
    status_buffer_t result = BUFFER_SUCCESS;
    
    if (get_busy_len_buf(buf) >= size){
        if (readed_byte != NULL){
            while(size--) {                                 // for (int i = buf->tail; (buf->tail - i) < size; i+=1){
                *(readed_byte++) = buf->byte[buf->head++];
                if (buf->head == BUF_LENGHT) buf->head = 0;
            }
        } 
        else {
            if ((buf->head + size) >= BUF_LENGHT) buf->head = (buf->head + size) - BUF_LENGHT;
            else  buf->head = (buf->head + size);
        }
    } else {
        result = BUFFER_EMPTY;
    }

    return result;
}


unsigned int get_busy_len_buf(cycle_buf_t *buf){
    int lenght = 0;
    if (buf->head > buf->tail) {
        lenght = (BUF_LENGHT - buf->head) + buf->tail;      // for example: (190 - 155) + 14 = 49 --> [155, 156, .. 189, 0, 1, .. 13] - busy, [14, .. 154] - empty.
    } else {
        lenght = buf->tail - buf->head;                     // for example: 27 - 11 = 16 --> [11, 12, ... 26] - busy,  [27, .. 189, 0, .. 10] - empty.
    }
    return lenght;
}

unsigned int get_empty_len_buf(cycle_buf_t *buf){
    return BUF_LENGHT - get_busy_len_buf(buf) - 1;
}

//-------------------------
// additional function 
//-------------------------

unsigned int get_busy_len_to_the_end_buf(cycle_buf_t *buf){
    int lenght = 0;
    if (buf->head > buf->tail) {
        lenght = (BUF_LENGHT - buf->head);      
    } else {
        lenght = buf->tail - buf->head;         
    }
    return lenght;
}
