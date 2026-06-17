/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ringbuffer.c
  * @brief   This file provides code for the ringbuffer.
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
#include "ringbuffer.h"

// 定义环形缓冲区大小
#define RB_SIZE 64U

// 定义环形缓冲区
static uint8_t rb_buf[RB_SIZE];

// 定义环形缓冲区头指针
static volatile uint8_t rb_head = 0U;

// 定义环形缓冲区尾指针
static volatile uint8_t rb_tail = 0U;

void rb_write(uint8_t data)
{
  uint8_t next = (rb_head + 1U) % RB_SIZE;
  if (next != rb_tail)
  {
    rb_buf[rb_head] = data;
    rb_head = next;
  }
}

uint8_t rb_read(uint8_t *data)
{
  if (rb_head == rb_tail)
  {
    return 0U;
  }
  *data = rb_buf[rb_tail];
  rb_tail = (rb_tail + 1U) % RB_SIZE;
  return 1U;
}

uint8_t rb_available(void)
{
  return (RB_SIZE + rb_head - rb_tail) % RB_SIZE;
}

