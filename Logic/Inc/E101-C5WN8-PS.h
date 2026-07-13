/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    E101-C5WN8-PS.h
  * @brief   This file contains all the function prototypes for
  *          the E101-C5WN8-PS.c file
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
#ifndef __E101_C5WN8_PS_H__
#define __E101_C5WN8_PS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>
#include "ringbuffer.h"

/** LED数量，两个TLC59116，每个连接8个LED */
#define LED_COUNT 16U

/** 每个LED的通道数量（每个LED有2个波长的通道） */
#define CHANNEL_COUNT_PER_LED 2U

/** 接收器数量，两个ADS1299，每个连接8个接收器 */
#define RECVIVER_COUNT 16U

/** 命令头 */
#define CMD_HEADER 0x846F

/** wifi usart3接收缓存 */
extern volatile uint8_t g_wifi_usart3_rx_byte;

/** wifi 环形缓冲区 */
extern RingBuffer g_wifi_rb;

/* 以下是解析出来的命令参数 ------------------------------------------------------------------*/

/**  时间戳, 上位机应该传0，下位机每隔100us增加1 */
extern volatile uint32_t g_timestamp;
/**  光源的亮度 */
extern uint8_t g_led_brightness[LED_COUNT];
/**  通道标志 */
extern uint8_t g_channel_mask[LED_COUNT * RECVIVER_COUNT / 8];

/**  开关机标志是否改变 */
extern bool g_power_flag_changed;
/**  开关机标志 */
extern bool g_power_flag;

/**  离线存储标志是否改变 */
extern bool g_storage_flag_changed;
/**  离线存储标志 */
extern bool g_storage_flag;

/**  采样率是否改变 */
extern bool g_sample_rate_changed;
/**  采样率 */
extern uint8_t g_sample_rate;

/**  开始采样标志是否改变 */
extern bool g_start_flag_changed;
/**  开始采样标志 */
extern bool g_start_flag;

/*
 * @brief 初始化WIFI模块
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-13
*/
void WIFI_Init(void);

/*
 * @brief 组装命令
 *
 * @param uint8_t byte 要处理的字节
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
*/
void WIFI_AssembleCommand(uint8_t byte);

/*
 * @brief 解析命令
 *
 * @param uint8_t* cmd 要解析的命令指针
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-17
*/
void WIFI_ParseCommand(uint8_t* cmd);

#ifdef __cplusplus
}
#endif
#endif /*__E101_C5WN8_PS_H__ */
