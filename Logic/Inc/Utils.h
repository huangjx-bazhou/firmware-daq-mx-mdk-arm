/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Utils.h
  * @brief   This file contains all the function prototypes for
  *          the Utils.c file
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
#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief  初始化DWT计时器
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-29
 */
void DWT_Init(void);

/**
 * @brief  延时指定微秒数
 * @param[in] us 微秒数
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-29
 */
void Delay_US(uint32_t us);

/**
 * @brief  检查指定位是否为1
 * @param[in] value 要检查的8位值
 * @param[in] bit 要检查的位（0-7）
 * @return bool 位为1则返回true，否则返回false
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-01
 */
bool Is_Bit_Set(uint8_t value, uint8_t bit);

/**
 * @brief  将24位有符号整数（3字节大端序）符号扩展为32位有符号整数
 * @param[in] raw 指向3字节数组的指针
 *                 raw[0] = 高字节（符号位在此）
 *                 raw[1] = 中字节
 *                 raw[2] = 低字节
 * @return int32_t 符号扩展后的32位有符号整数值
 *
 * @note   适用于ADS1299等24位ΔΣ ADC的数据格式转换
 *         算法：先左移24位使符号位对齐到Bit31，再算术右移8位实现符号扩展
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-13
 */
int32_t Sign_Extend_24_to_32(const uint8_t raw[3]);

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_H__ */

