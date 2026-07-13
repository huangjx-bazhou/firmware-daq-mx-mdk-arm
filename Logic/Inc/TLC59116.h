/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tlc59116.h
  * @brief   This file contains all the function prototypes for
  *          the tlc59116.c file
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
#ifndef __TLC59116_H__
#define __TLC59116_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN Prototypes */

/**
 * @brief 写入指定寄存器
 *
 * @param addr7   TLC59116从设备地址
 * @param reg     寄存器地址
 * @param value   寄存器值
 * @return HAL_StatusTypeDef
 *         - HAL_OK: 成功
 *         - HAL_ERROR: 发生错误
 *         - HAL_BUSY: 设备忙
 *         - HAL_TIMEOUT: 操作超时
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-11
 */
HAL_StatusTypeDef TLC59116_WriteReg(uint8_t addr7, uint8_t reg, uint8_t value);

/**
 * @brief 初始化TLC59116为PWM模式，设置所有通道的PWM值为0
 *
 * @param addr7   TLC59116从设备地址
 * @return HAL_StatusTypeDef
 *         - HAL_OK: 成功
 *         - HAL_ERROR: 发生错误
 *         - HAL_BUSY: 设备忙
 *         - HAL_TIMEOUT: 操作超时
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-11
 */
HAL_StatusTypeDef TLC59116_InitPwmMode(uint8_t addr7);

/**
 * @brief 初始化所有TLC59116为PWM模式，设置所有通道的PWM值为0
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-11
 */
void TLC59116_Init(void);

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

/**
 * @brief 设置第1个TLC59116指定通道的PWM值
 *
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
 * @date    2026-06-11
 */
HAL_StatusTypeDef TLC59116_1_SetPwm(uint8_t channel, uint8_t pwm);

/**
 * @brief 设置第2个TLC59116指定通道的PWM值
 *
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
 * @date    2026-06-11
 */
HAL_StatusTypeDef TLC59116_2_SetPwm(uint8_t channel, uint8_t pwm);

/**
 * @brief 设置所有通道的PWM值
 *
 * @param addr7   I2C从设备地址
 * @param pwm     PWM值，范围0-255
 * @return HAL_StatusTypeDef
 *         - HAL_OK: 成功
 *         - HAL_ERROR: 发生错误
 *         - HAL_BUSY: 设备忙
 *         - HAL_TIMEOUT: 操作超时
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-11
 */
HAL_StatusTypeDef TLC59116_SetAllPwm(uint8_t addr7, uint8_t pwm);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TLC59116_H__ */
