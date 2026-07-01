/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    utils.c
  * @brief   This file provides code for the utils tools.
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
#include "utils.h"

void DWT_Init(void)
{
    /// TODO: AI生成，待验证
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
}

void Delay_US(uint32_t us)
{
    /// TODO: 待优化

    /// NOTE: DWT计数不受中断影响，所以这个延时可以被中断拉长，导致延时不准
    uint32_t start = DWT->CYCCNT;

    /// NOTE: SystemCoreClock一般都是 X MHz，这里总是会整除，如果不是1M的整数倍，请将
    /// MCU系统时钟设置为1M的整数倍
    uint32_t ticks = us * (SystemCoreClock / 1000000U);

    /// NOTE: 使用有符号特性防止计数溢出，如果回绕了两圈怎么办
    while ((int32_t)(DWT->CYCCNT - start) < (int32_t)ticks);
}
