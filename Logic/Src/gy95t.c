/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gy95t.c
  * @brief   This file provides code for the acquisition
  *          of gy95t.
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

void GY95T_Init(void)
{
  uint8_t cmd[] = { 0x00, 0x06, 0x03, 0x00, 0x09 };
  HAL_UART_Transmit(&huart2, cmd, sizeof(cmd) / sizeof(cmd[0]), 100U);
}
