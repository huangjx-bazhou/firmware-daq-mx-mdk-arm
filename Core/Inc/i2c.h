/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
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
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c2;

/* USER CODE BEGIN Private defines */
#define TLC59116_ADDR_1   0x60U
#define TLC59116_ADDR_2   0x61U
/* USER CODE END Private defines */

void MX_I2C2_Init(void);

/* USER CODE BEGIN Prototypes */

HAL_StatusTypeDef TLC59116_InitPwmMode(uint8_t addr7);
HAL_StatusTypeDef TLC59116_SetAllPwm(uint8_t addr7, uint8_t pwm);

/**
 * @brief 设置指定通道的PWM值
 *
 * @param addr7   I2C从设备地址
 * @param channel 通道号0-15
 * @param pwm     PWM值，范围0-255
 * @return HAL_StatusTypeDef
 *         - HAL_OK: 成功
 *         - HAL_ERROR: 发生错误
 *         - HAL_BUSY: 设备忙
 *         - HAL_TIMEOUT: 操作超时
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-04
 */
HAL_StatusTypeDef TLC59116_SetPwm(uint8_t addr7, uint8_t channel, uint8_t pwm);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

