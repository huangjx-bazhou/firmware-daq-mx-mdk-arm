/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Power_Pin GPIO_PIN_13
#define Power_GPIO_Port GPIOC
#define BatteryLevel_Pin GPIO_PIN_2
#define BatteryLevel_GPIO_Port GPIOC
#define ADS1299_2_DRDY_Pin GPIO_PIN_0
#define ADS1299_2_DRDY_GPIO_Port GPIOA
#define ADS1299_1_CS_Pin GPIO_PIN_3
#define ADS1299_1_CS_GPIO_Port GPIOA
#define ADS1299_2_RESET_Pin GPIO_PIN_5
#define ADS1299_2_RESET_GPIO_Port GPIOA
#define TLC59116_2_RESET_Pin GPIO_PIN_6
#define TLC59116_2_RESET_GPIO_Port GPIOA
#define ADS1299_1_DRDY_Pin GPIO_PIN_0
#define ADS1299_1_DRDY_GPIO_Port GPIOB
#define ADS1299_START_Pin GPIO_PIN_8
#define ADS1299_START_GPIO_Port GPIOE
#define TLC59116_1_RESET_Pin GPIO_PIN_12
#define TLC59116_1_RESET_GPIO_Port GPIOB
#define WIFI_USART3_TX_Pin GPIO_PIN_8
#define WIFI_USART3_TX_GPIO_Port GPIOD
#define WIFI_USART3_RX_Pin GPIO_PIN_9
#define WIFI_USART3_RX_GPIO_Port GPIOD
#define GY95T_USART2_TX_Pin GPIO_PIN_5
#define GY95T_USART2_TX_GPIO_Port GPIOD
#define GY95T_USART2_RX_Pin GPIO_PIN_6
#define GY95T_USART2_RX_GPIO_Port GPIOD
#define ADS1299_2_CS_Pin GPIO_PIN_7
#define ADS1299_2_CS_GPIO_Port GPIOD
#define ADS1299_1_RESET_Pin GPIO_PIN_7
#define ADS1299_1_RESET_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
