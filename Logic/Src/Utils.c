/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Utils.c
  * @brief   This file provides code for the Utils tools.
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
#include "Utils.h"
#include "main.h"

void DWT_Init(void)
{
  /* TODO: AI生成，待验证 */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
}

void Delay_US(uint32_t us)
{
  /* TODO: 待优化 */

  /* NOTE: DWT计数不受中断影响，所以这个延时可以被中断拉长，导致延时不准 */
  uint32_t start = DWT->CYCCNT;

  /* NOTE: SystemCoreClock一般都是 X MHz，这里总是会整除，如果不是1M的整数倍，请将MCU系统时钟设置为1M的整数倍 */
  uint32_t ticks = us * (SystemCoreClock / 1000000U);

  /* NOTE: 使用有符号特性防止计数溢出，如果回绕了两圈怎么办 */
  while ((int32_t)(DWT->CYCCNT - start) < (int32_t)ticks);
}

bool Is_Bit_Set(uint8_t value, uint8_t bit)
{
  /* 7 6 5 4 3 2 1 0  位从高到低编号 */
  if (bit > 7) return false;
  return (value & (1 << bit)) != 0U;
}

int32_t Sign_Extend_24_to_32(const uint8_t raw[3])
{
    return ((int32_t)(
        ((uint32_t)raw[0] << 24) |
        ((uint32_t)raw[1] << 16) |
        ((uint32_t)raw[2] <<  8)
    )) >> 8;
}
