/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    parsecmd.h
  * @brief   This file contains all the function prototypes for
  *          the parsecmd.c file
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
#ifndef __PARSECMD_H__
#define __PARSECMD_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
// LED数量，两个TLC59116，每个连接8个LED
#define LED_COUNT 16U

// 接收器数量，两个ADS1299，每个连接8个接收器
#define RECVIVER_COUNT 16U

// 命令头
#define CMD_HEADER 0x846F

// 命令数据包长度
#define CMD_PACKET_SIZE 59U

#define CMD_ENABLE_FLAG 0x55

#define CMD_DISABLE_FLAG 0xAA

// 离线存储标志是否改变标志(0: 未改变, 1: 改变)
extern uint8_t g_storage_flag_changed;

// 采样率是否改变标志(0: 未改变, 1: 改变)
extern uint8_t g_sample_rate_changed;

// 开始采样标志是否改变标志(0: 未改变, 1: 已改变)
extern uint8_t g_start_flag_changed;

extern uint32_t g_timestamp;                                     // 时间戳
extern uint8_t  g_led_brightness[LED_COUNT];                     // 光源的亮度
extern uint8_t  g_channel_mask[LED_COUNT * RECVIVER_COUNT / 8];  // 16个光源和16个接收器的通道标志
extern uint8_t  g_power_flag;                  // 开关机标志
extern uint8_t  g_storage_flag;               // 离线存储标志
extern uint8_t  g_sample_rate;                             // 采样率
extern uint8_t  g_start_flag;                 // 开始采样标志

/*
 * @brief 解析命令
 *
 * @param uint8_t* cmd 要解析的命令指针
 * @return uint8_t 解析结果
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-17
*/
uint8_t parse_cmd(uint8_t* cmd);

#ifdef __cplusplus
}
#endif
#endif /*__PARSECMD_H__ */

