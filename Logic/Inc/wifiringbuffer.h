/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    wifiringbuffer.h
  * @brief   This file contains all the function prototypes for
  *          the wifiringbuffer.c file
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
#ifndef __WIFIRINGBUFFER_H__
#define __WIFIRINGBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*
 * @brief 写入数据到环形缓冲区
 * @param data 要写入的数据
 * @return void
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-17
*/
void wifi_rb_write(uint8_t data);

/*
 * @brief 从环形缓冲区读取数据
 * @param data 读取到的数据指针
 * @return uint8_t 读取到的数据长度
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-17
 * @return uint8_t 读取到的数据长度
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-17
*/
uint8_t wifi_rb_read(uint8_t *data);

/*
 * @brief 获取环形缓冲区中可用的字节数
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-17
*/
uint8_t wifi_rb_available(void);

#ifdef __cplusplus
}
#endif
#endif /*__WIFIRINGBUFFER_H__ */

