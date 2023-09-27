#ifndef UART_H
#define UART_H

#include "main.h"
#include "cycle_buffer.h"


typedef struct {
    // cycle_buf_t rx_buf;
    uint8_t rx_packet[1];
    osMessageQueueId_t * rx_queue;
    cycle_buf_t tx_buf;
    
    // volatile uint16_t rx_asked_bytes;
    volatile uint16_t tx_len_sended;
    UART_HandleTypeDef * uart_hand;
    osEventFlagsId_t  * flag_event;
} uart_exchange_t;


extern uart_exchange_t encrypt_uart;
extern uart_exchange_t decrypt_uart;

void exchange(uart_exchange_t * dev);
void can_send_next_byte(uart_exchange_t *dev);

#endif
