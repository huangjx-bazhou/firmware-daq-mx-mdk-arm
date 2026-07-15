/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    E101-C5WN8-PS.c
  * @brief   This file provides code for the E101-C5WN8-PS.
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
#include "E101-C5WN8-PS.h"
#include <string.h>

#define CMD_PACKET_SIZE 59U
#define CMD_ENABLE_FLAG 0x55
#define CMD_DISABLE_FLAG 0xAA
#define RECEIVED_NONE 0U
#define RECEIVED_84   1U
#define RECEIVED_6F   2U

volatile uint8_t g_wifi_usart3_rx_byte = 0U;

RingBuffer g_wifi_rb;

/** wifi 命令 */
static uint8_t wifi_cmd[CMD_PACKET_SIZE];

/** 状态 */
static uint8_t wifi_received_state = RECEIVED_NONE;

/** 收到的数据长度 */
static uint8_t wifi_received = 0U;

volatile uint32_t g_timestamp = 0U;
uint8_t g_led_brightness[LED_COUNT];
uint8_t g_channel_mask[LED_COUNT * RECVIVER_COUNT / 8];
bool g_power_flag_changed = false;
bool g_power_flag = true;
bool g_storage_flag_changed = false;
bool g_storage_flag = false;
bool g_sample_rate_changed = false;
uint8_t g_sample_rate = 50U;
bool g_start_flag_changed = false;
bool g_start_flag = false;

void WIFI_Init(void)
{
  /* 初始化环形缓冲区 */
  RB_Init(&g_wifi_rb);

  /* 初始化LED亮度 */
  memset(g_led_brightness, 0xFF, LED_COUNT);

  /* TODO: 初始化通道掩码 */
}

void WIFI_AssembleCommand(uint8_t byte)
{
  switch (wifi_received_state)
  {
    case RECEIVED_NONE:
      if (0x84 == byte)
      {
        wifi_cmd[wifi_received++] = byte;
        wifi_received_state = RECEIVED_84;
      }
      break;
    case RECEIVED_84:
      if (0x6F == byte)
      {
        wifi_cmd[wifi_received++] = byte;
        wifi_received_state = RECEIVED_6F;
      }
      else
      {
        wifi_received = 0;
        wifi_received_state = RECEIVED_NONE;
      }
      break;
    case RECEIVED_6F:
      wifi_cmd[wifi_received++] = byte;
      if (wifi_received >= CMD_PACKET_SIZE)
      {
        WIFI_ParseCommand(wifi_cmd);
        wifi_received_state = RECEIVED_NONE;
      }
      break;
  }
}

void WIFI_ParseCommand(uint8_t* cmd)
{
  /* 不验证第0个字节和第1个字节是不是0x84和0x6F */

  /* 不处理第2字节，因为它是0x0B，不感兴趣 */

  /* 不处理第3，4，5，6字节，不感兴趣 */

  /* LED亮度解析 */
  for (uint8_t i = 0; i < LED_COUNT; ++i)
  {
    g_led_brightness[i] = cmd[7U + i];
  }

  /* 通道掩码复制 */
  memcpy(g_channel_mask, cmd + 23U, LED_COUNT * RECVIVER_COUNT / 8);

  /* 开关机标志解析 */
  if (cmd[55] != g_power_flag)
  {
    g_power_flag = cmd[55];
    g_power_flag_changed = true;
  }

  /* 离线存储标志解析 */
  if (cmd[56] != g_storage_flag)
  {
    g_storage_flag = cmd[56];
    g_storage_flag_changed = true;
  }

  /* 采样率解析 */
  if (cmd[57] != g_sample_rate)
  {
    g_sample_rate = cmd[57];
    g_sample_rate_changed = true;
  }

  /* 开始采样标志解析 */
  if (cmd[58] != g_start_flag)
  {
    g_start_flag = cmd[58];
    g_start_flag_changed = true;
  }
}
