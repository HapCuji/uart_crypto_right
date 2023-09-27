/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdint.h"
#include "stdbool.h"

#include "cmsis_os.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define MAX_TIM2_COUNTER    0xFFFF			  // ms
#define MAX_TIM5_COUNTER    0xFFFFFFFF		// us

#define GET_TIME_US           __HAL_TIM_GET_COUNTER(&htim5)
#define MAX_COUNTER_US 	      MAX_TIM5_COUNTER
#define GET_TIME_MS           __HAL_TIM_GET_COUNTER(&htim2)
#define MAX_COUNTER_MS 		    MAX_TIM2_COUNTER
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5; 
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CPU_FREQ_HZ 32000000
/* USER CODE BEGIN Private defines */
// extern void encrypter_task(void *argument);
// extern void decrypter_task(void *argument);
extern osEventFlagsId_t encrypted_readyHandle;
extern osEventFlagsId_t decrypted_readyHandle;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
