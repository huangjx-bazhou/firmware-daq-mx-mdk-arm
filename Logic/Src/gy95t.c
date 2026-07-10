/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gy95t.c
  * @brief   This file provides code for the acquisition of gy95t.
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
#include "gy95t.h"

// 连续输出命令
const uint8_t gy95t_start_cmd[] = { 0x00, 0x06, 0x03, 0x00, 0x09 };

// 查询输出命令
const uint8_t gy95t_stop_cmd[] = { 0x00, 0x06, 0x03, 0x01, 0x0A };

// 超时时间
const uint32_t gy95t_timeout = 100U;

// usart2接收缓存
volatile uint8_t g_usart2_rx_byte;

void GY95T_Start(void)
{
  HAL_UART_Transmit(&huart2, gy95t_start_cmd, sizeof(gy95t_start_cmd), gy95t_timeout);
}

void GY95T_Stop(void)
{
  HAL_UART_Transmit(&huart2, gy95t_stop_cmd, sizeof(gy95t_stop_cmd), gy95t_timeout);
}
