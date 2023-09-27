#include "uart.h"

uart_exchange_t encrypt_uart = {
    .rx_packet = {0},
    .rx_queue = &encrypted_dataHandle,
    .tx_buf = {
        .byte = {0},
        .tail = 0,
        .head = 0
    },
    .tx_len_sended = 0,
    .uart_hand = &huart1,
    .flag_event = &encrypted_readyHandle
};
uart_exchange_t decrypt_uart = {
    .rx_packet = {0},
    .rx_queue = &decrypted_dataHandle,
    .tx_buf = {
        .byte = {0},
        .tail = 0,
        .head = 0
    },
    .tx_len_sended = 0,
    .uart_hand = &huart2,
    .flag_event = &decrypted_readyHandle
};

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_exchange_t * dev = (huart == encrypt_uart.uart_hand)? &encrypt_uart : \
                            (huart == decrypt_uart.uart_hand)? &decrypt_uart : NULL; 
    if (dev != NULL){
        // can_send_next_byte(dev);
        osEventFlagsSet(*dev->flag_event, 2);
    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_exchange_t * dev = (huart == encrypt_uart.uart_hand)? &encrypt_uart : \
                            (huart == decrypt_uart.uart_hand)? &decrypt_uart : NULL; 
    if (dev != NULL){
        osMessageQueuePut(*dev->rx_queue, dev->rx_packet, -1, 0); 			// -1 do not care priotity
        HAL_UART_Receive_IT(dev->uart_hand, dev->rx_packet, 1);
        
        
        osEventFlagsSet(*dev->flag_event, 1);
    }
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    uart_exchange_t * dev = (huart == encrypt_uart.uart_hand)? &encrypt_uart : \
                            (huart == decrypt_uart.uart_hand)? &decrypt_uart : NULL; 
    if (dev != NULL){
    	if (HAL_UART_GetError(dev->uart_hand) == HAL_UART_ERROR_ORE){
    		dev->rx_packet[0] = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
    		osMessageQueuePut(*dev->rx_queue, dev->rx_packet, -1, 0);	                // push error byte?
    	}
        HAL_UART_Receive_IT(dev->uart_hand, dev->rx_packet, 1);
    }
}


void can_send_next_byte(uart_exchange_t *dev){
    
    if (dev->tx_len_sended != 0){
        pull_bytes(&dev->tx_buf, NULL, dev->tx_len_sended);                // free already sended bytes
    }
    dev->tx_len_sended = get_busy_len_to_the_end_buf(&(dev->tx_buf));

    if (dev->tx_len_sended > 0){
        osEventFlagsClear(*dev->flag_event, 2);
        if (HAL_UART_Transmit_DMA(dev->uart_hand, (uint8_t *)(dev->tx_buf.byte + dev->tx_buf.head), dev->tx_len_sended) == HAL_OK){ // != HAL_BUSY
        } else {
            osEventFlagsSet(*dev->flag_event, 2);
        	dev->tx_len_sended = 0;                 // must not be HAL_BUSY check it
        }
    } else {
        dev->tx_len_sended = 0;
    }

}

void exchange(uart_exchange_t * dev){
    static uint8_t new_packet[1] = {0};
    
    HAL_UART_Receive_IT(dev->uart_hand, dev->rx_packet, 1);

    if (dev == &encrypt_uart){
        while (osMessageQueueGet(*dev->rx_queue, new_packet, NULL, 1) == osOK){ // check timeout, better set '0'
           push_bytes(&decrypt_uart.tx_buf, new_packet, 1);
           osEventFlagsSet(*decrypt_uart.flag_event, 4);
        }
    }
    else if (dev == &decrypt_uart){
        while (osMessageQueueGet(*dev->rx_queue, new_packet, NULL, 1) == osOK){
           push_bytes(&encrypt_uart.tx_buf, new_packet, 1);
           osEventFlagsSet(*encrypt_uart.flag_event, 4);
        }
    }

    //  if ((dev->tx_len_sended == 0) && (get_busy_len_buf(&dev->tx_buf) > 0) ){
    if ( (osEventFlagsGet(*dev->flag_event) & 0x02) || (dev->uart_hand->gState == HAL_UART_STATE_READY) ){
        can_send_next_byte(dev);
     }
    
}
