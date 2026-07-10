/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gy95tringbuffer.c
  * @brief   This file provides code for the gy95tringbuffer.
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
#include "gy95tringbuffer.h"

// 定义环形缓冲区大小
#define GY95T_RB_SIZE 64U

// 定义环形缓冲区
volatile uint8_t gy95t_rb_buf[GY95T_RB_SIZE];

// 定义环形缓冲区头指针
volatile uint8_t gy95t_rb_head = 0U;

// 定义环形缓冲区尾指针
volatile uint8_t gy95t_rb_tail = 0U;

void gy95t_rb_write(uint8_t data)
{
  uint8_t next = (gy95t_rb_head + 1U) % GY95T_RB_SIZE;
  if (next != gy95t_rb_tail)
  {
    gy95t_rb_buf[gy95t_rb_head] = data;
    gy95t_rb_head = next;
  }
}

uint8_t gy95t_rb_read(uint8_t *data)
{
  if (gy95t_rb_head == gy95t_rb_tail)
  {
    return 0U;
  }
  *data = gy95t_rb_buf[gy95t_rb_tail];
  gy95t_rb_tail = (gy95t_rb_tail + 1U) % GY95T_RB_SIZE;
  return 1U;
}

uint8_t gy95t_rb_available(void)
{
  return (GY95T_RB_SIZE + gy95t_rb_head - gy95t_rb_tail) % GY95T_RB_SIZE;
}
