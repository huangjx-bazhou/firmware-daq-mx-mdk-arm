/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "gpio.h"
/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_SPI1_Init(void);

/* USER CODE BEGIN Prototypes */

/********************************************************
 * ADS1299芯片相关函数
 ********************************************************/

/**
 * @brief   向第1个ADS1299芯片发送命令字节
 *
 * @param   cmd 要发送的命令字节
 * @return  HAL_StatusTypeDef 发送状态
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-08
 */
HAL_StatusTypeDef ADS1299_1_SendCmd(uint8_t cmd);

/**
 * @brief   向第1个ADS1299芯片写入寄存器
 *
 * @param   reg 寄存器地址
 * @param   value 要写入的值字节
 * @return  HAL_StatusTypeDef 发送状态
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-08
 */
HAL_StatusTypeDef ADS1299_1_WriteReg(uint8_t reg, uint8_t value);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

