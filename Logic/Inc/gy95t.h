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

#include "usart.h"

// adc3的采样值
extern uint32_t g_adc3_sample;

/*
 * @brief 初始化GY95T
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-06
 */
void GY95T_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __GY95T_H__ */
