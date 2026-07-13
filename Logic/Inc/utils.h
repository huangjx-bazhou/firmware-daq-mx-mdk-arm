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
 * @param us 微秒数
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-29
 */
void Delay_US(uint32_t us);

/**
 * @brief  检查指定位是否为1
 * @param value 要检查的8位值
 * @param bit 要检查的位（0-7）
 * @return bool 位为1则返回true，否则返回false
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-01
 */
bool Is_Bit_Set(uint8_t value, uint8_t bit);

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_H__ */
