/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    parsecmd.c
  * @brief   This file provides code for the parsecmd.
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

/* Includes ------------------------------------------------------------------*/
#include "parsecmd.h"
#include <string.h>

// 离线存储标志是否改变标志(0: 未改变, 1: 改变)
uint8_t g_storage_flag_changed = 0U;

// 采样率是否改变标志(0: 未改变, 1: 改变)
uint8_t g_sample_rate_changed = 0U;

// 开始采样标志是否改变标志(0: 未改变, 1: 已改变)
uint8_t g_start_flag_changed = 0U;

uint32_t g_timestamp;                                     // 时间戳
uint8_t  g_led_brightness[LED_COUNT];                     // 光源的亮度
uint8_t  g_channel_mask[LED_COUNT * RECVIVER_COUNT / 8];  // 16个光源和16个接收器的通道标志
uint8_t  g_power_flag = CMD_ENABLE_FLAG;                  // 开关机标志
uint8_t  g_storage_flag = CMD_DISABLE_FLAG;               // 离线存储标志
uint8_t  g_sample_rate = 10U;                             // 采样率
uint8_t  g_start_flag = CMD_DISABLE_FLAG;                 // 开始采样标志

uint8_t parse_cmd(uint8_t* cmd)
{
  // TODO: 验证头是否解析正确
//  if ((uint16_t)cmd[0] != CMD_HEADER)
//  {
//    return 1U;
//  }

  // 不处理第3字节，因为它是0x0B，不感兴趣

  // TODO: 验证时间戳是否解析正确
  g_timestamp = (uint32_t)cmd[3];

  // LED亮度解析
  for (uint8_t i = 0; i < LED_COUNT; i++)
  {
    g_led_brightness[i] = cmd[7 + i];
  }

  // 通道掩码复制
  memcpy(g_channel_mask, cmd + 23, LED_COUNT * RECVIVER_COUNT / 8);

  // 开关机标志
  g_power_flag = cmd[55];

  // 离线存储标志解析
  if (cmd[56] != g_storage_flag)
  {
    g_storage_flag = cmd[56];
    g_storage_flag_changed = 1;
  }
  
    // 采样率解析
  if (cmd[57] != g_sample_rate)
  {
    g_sample_rate = cmd[57];
    g_sample_rate_changed = 1;
  }
  
  // 开始采样标志解析
  if (cmd[58] != g_start_flag)
  {
    g_start_flag = cmd[58];
    g_start_flag_changed = 1;
  }
	
	return 0U;
}

