/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    GY-95T.c
  * @brief   This file provides code for the acquisition of GY-95T.
  *
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
#include "GY-95T.h"

/** 连续输出命令 */
static const uint8_t gy95t_start_cmd[] = { 0x00, 0x06, 0x03, 0x00, 0x09 };

/** 查询输出命令 */
static const uint8_t gy95t_stop_cmd[] = { 0x00, 0x06, 0x03, 0x01, 0x0A };

/** 超时时间100ms */
static const uint32_t gy95t_timeout = 100U;

/** GY-95T usart2接收缓存 */
volatile uint8_t g_gy95t_usart2_rx_byte;

/** GY-95T 环形缓冲区 */
RingBuffer g_gy95t_rb;

/** GY-95T 数据缓存 */
uint8_t g_gy95t_data[GY95T_DATA_SIZE];

#define RECEIVED_NONE 0U
#define RECEIVED_A4   1U
#define RECEIVED_03   2U
#define RECEIVED_08   3U
#define RECEIVED_1B   4U

/** 状态 */
static uint8_t gy95t_received_state = RECEIVED_NONE;

/** 收到的数据长度 */
static uint8_t gy95t_received = 0U;

void GY95T_Init(void)
{
  RB_Init(&g_gy95t_rb);
}

void GY95T_Start(void)
{
  HAL_UART_Transmit(&huart2, gy95t_start_cmd, sizeof(gy95t_start_cmd), gy95t_timeout);
}

void GY95T_Stop(void)
{
  HAL_UART_Transmit(&huart2, gy95t_stop_cmd, sizeof(gy95t_stop_cmd), gy95t_timeout);
}

void GY95T_ProcessData(uint8_t data)
{
  switch (gy95t_received_state)
  {
    case RECEIVED_NONE:
      gy95t_received_state = 0xA4 == data ? RECEIVED_A4 : RECEIVED_NONE;
      break;
    case RECEIVED_A4:
      gy95t_received_state = 0x03 == data ? RECEIVED_03 : RECEIVED_NONE;
      break;
    case RECEIVED_03:
      gy95t_received_state = 0x08 == data ? RECEIVED_08 : RECEIVED_NONE;
      break;
    case RECEIVED_08:
      gy95t_received_state = 0x1B == data ? RECEIVED_1B : RECEIVED_NONE;
      break;
    case RECEIVED_1B:
      if (gy95t_received >= 28U)
      {
        gy95t_received = 0U;
        gy95t_received_state = RECEIVED_NONE;
      }
      else if (gy95t_received >= GY95T_DATA_SIZE)
      {
        gy95t_received++;
      }
      else
      {
        g_gy95t_data[gy95t_received++] = data;
      }
      break;
    default:
      break;
  }
}
