#ifndef CYCLE_BUFFER
#define CYCLE_BUFFER

#include "main.h"

#define BUF_LENGHT  120

typedef enum {
    BUFFER_SUCCESS,
    BUFFER_FULL,
    BUFFER_EMPTY,
    BUFFER_ERROR
} status_buffer_t;

typedef struct {
    volatile unsigned char byte [BUF_LENGHT];
    volatile unsigned short head;
    volatile unsigned short tail;
} cycle_buf_t;

status_buffer_t init_and_clean_buf(cycle_buf_t *buf);
unsigned int get_empty_len_buf(cycle_buf_t *buf);
unsigned int get_busy_len_buf(cycle_buf_t *buf);
status_buffer_t pull_bytes(cycle_buf_t * buf, unsigned char * readed_byte, unsigned short size);
status_buffer_t push_bytes(cycle_buf_t * buf, unsigned char * new_byte, unsigned short size);

unsigned int get_busy_len_to_the_end_buf(cycle_buf_t *buf);

#endif