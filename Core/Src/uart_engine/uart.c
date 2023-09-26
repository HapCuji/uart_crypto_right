#include "uart.h"

uart_exchange_t encrypt_uart = {
    .rx_buf = {
        .byte = {0},
        .tail = 0,
        .head = 0
    },
    .tx_buf = {
        .byte = {0},
        .tail = 0,
        .head = 0
    },
    .tx_len_sended = 0,
    .uart_hand = &huart1
};
uart_exchange_t decrypt_uart = {
    .rx_buf = {
        .byte = {0},
        .tail = 0,
        .head = 0
    },
    .tx_buf = {
        .byte = {0},
        .tail = 0,
        .head = 0
    },
    .tx_len_sended = 0,
    .uart_hand = &huart2
};

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_exchange_t * dev = (huart == encrypt_uart.uart_hand)? &encrypt_uart : \
                            (huart == decrypt_uart.uart_hand)? &decrypt_uart : NULL; 
    if (dev != NULL){
        can_send_next_byte(dev);
    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_exchange_t * dev = (huart == encrypt_uart.uart_hand)? &encrypt_uart : \
                            (huart == decrypt_uart.uart_hand)? &decrypt_uart : NULL; 
    if (dev != NULL){
		push_bytes(&dev->rx_buf, NULL, 1); 											
        HAL_UART_Receive_IT(dev->uart_hand, (uint8_t *)(dev->rx_buf.byte + dev->rx_buf.tail), 1);
    }
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    uart_exchange_t * dev = (huart == encrypt_uart.uart_hand)? &encrypt_uart : \
                            (huart == decrypt_uart.uart_hand)? &decrypt_uart : NULL; 
    if (dev != NULL){
		if (dev->uart_hand->RxXferCount == 0) push_bytes(&dev->rx_buf, NULL, 1); 	                // push error byte?										
        HAL_UART_Receive_IT(dev->uart_hand, (uint8_t *)(dev->rx_buf.byte + dev->rx_buf.tail), 1);
    }
}


void can_send_next_byte(uart_exchange_t *dev){
    
    if (dev->tx_len_sended != 0){
        pull_bytes(&dev->tx_buf, NULL, dev->tx_len_sended);                // free already sended bytes
    }
    dev->tx_len_sended = get_busy_len_to_the_end_buf(&(dev->tx_buf));

    if (dev->tx_len_sended >= BUF_LENGHT/4){
        dev->tx_len_sended = BUF_LENGHT/4;
        if (HAL_UART_Transmit_DMA(dev->uart_hand, (uint8_t *)(dev->tx_buf.byte + dev->tx_buf.head), dev->tx_len_sended) == HAL_OK){ // != HAL_BUSY
        } else {
        	dev->tx_len_sended = 0;                 // must not be HAL_BUSY check it
        }
    } else {
        dev->tx_len_sended = 0;
    }

}

void exchange(uart_exchange_t * dev){
    uint8_t byte = 0;
    
    HAL_UART_Receive_IT(dev->uart_hand, (uint8_t *)(dev->rx_buf.byte + dev->rx_buf.tail), 1);

    if (dev == &encrypt_uart){
        while (pull_bytes(&dev->rx_buf, &byte, 1) == BUFFER_SUCCESS){
            push_bytes(&decrypt_uart.tx_buf, &byte, 1);
        }
    }
    else if (dev == &decrypt_uart){
        while (pull_bytes(&dev->rx_buf, &byte, 1) == BUFFER_SUCCESS){
            push_bytes(&encrypt_uart.tx_buf, &byte, 1);
        }
    }

    if ((dev->tx_len_sended == 0) && (get_busy_len_buf(&dev->tx_buf) > 0) ){
        can_send_next_byte(dev);
    }
}