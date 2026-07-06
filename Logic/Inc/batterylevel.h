/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    batterylevel.h
  * @brief   This file contains all the function prototypes for
  *          the batterylevel.c file
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
#ifndef __BATTERYLEVEL_H__
#define __BATTERYLEVEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "adc.h"

// adc3的采样值
extern uint32_t g_adc3_sample;

// 电压，根据adc3的采样值计算得到
extern float g_battery_voltage;

// 电量，由电压值计算得到
extern uint8_t g_battery_level;

/*
 * @brief 获取adc3的采样值，结果保存到g_adc3_sample并返回
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-24
 */
uint32_t get_adc3_sample(void);

/*
 * @brief 获取电池电压，结果保存到g_battery_voltage并返回
 *        内部调用get_adc3_sample获取adc3的采样值，再根据采样值计算电压值
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-24
 */
float get_battery_voltage(void);

/*
 * @brief 获取电池电量，结果保存到g_battery_level并返回
 *        内部调用get_battery_voltage获取电池电压，再根据电压值计算电量值
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-24
 */
uint8_t get_battery_level(void);

#ifdef __cplusplus
}
#endif

#endif /* __BATTERYLEVEL_H__ */
