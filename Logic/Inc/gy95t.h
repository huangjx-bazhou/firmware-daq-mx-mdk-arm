/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gy95t.h
  * @brief   This file contains all the function prototypes for
  *          the gy95t.c file
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
#ifndef __GY95T_H__
#define __GY95T_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ringbuffer.h"
#include "usart.h"

#define GY95T_DATA_SIZE 18U

/** gy95t usart2接收缓存 */
extern volatile uint8_t g_gy95t_usart2_rx_byte;

/** gy95t 环形缓冲区 */
extern RingBuffer g_gy95t_rb;

/** gy95t 数据缓存 */
extern uint8_t g_gy95t_data[GY95T_DATA_SIZE];

/*
 * @brief GY95T初始化
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
void GY95T_Init(void);

/*
 * @brief GY95T开始连续输出
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-06
 */
void GY95T_Start(void);

/*
 * @brief GY95T停止连续输出
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-09
 */
void GY95T_Stop(void);

/*
 * @brief GY95T处理一字节数据
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
void GY95T_ProcessData(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* __GY95T_H__ */
