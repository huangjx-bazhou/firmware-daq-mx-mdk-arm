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
#include <string.h>

void RB_Init(RingBuffer *rb)
{
  rb->head = 0U;
  rb->tail = 0U;
}

uint32_t RB_Write(RingBuffer *rb, const uint8_t *data, uint32_t len)
{
  uint32_t free = RB_SIZE - RB_Available(rb) - 1U;
  uint32_t count = (free < len) ? free : len;

  // 第一段: head到数组末尾
  uint32_t first = RB_SIZE - rb->head;
  if (first > count)
  {
    first = count;
  }
  memcpy(&rb->buf[rb->head], data, first);

  // 第二段: 数组开头(回绕)
  if (first < count)
  {
    memcpy(rb->buf, &data[first], count - first);
  }

  rb->head = (rb->head + count) & RB_MASK;
  return count;
}

uint32_t RB_Read(RingBuffer *rb, uint8_t *data, uint32_t len)
{
  uint32_t available = RB_Available(rb);
  uint32_t count = (available < len) ? available : len;

  // 第一段: tail到数组末尾
  uint32_t first = RB_SIZE - rb->tail;
  if (first > count)
  {
    first = count;
  }
  memcpy(data, &rb->buf[rb->tail], first);

  // 第二段: 数组开头(回绕)
  if (first < count)
  {
    memcpy(&data[first], rb->buf, count - first);
  }

  rb->tail = (rb->tail + count) & RB_MASK;
  return count;
}

bool RB_WriteByte(RingBuffer *rb, uint8_t data)
{
  return RB_Write(rb, &data, 1U) == 1U;
}

bool RB_ReadByte(RingBuffer *rb, uint8_t *data)
{
  return RB_Read(rb, data, 1U) == 1U;
}

uint32_t RB_Available(const RingBuffer *rb)
{
  return (RB_SIZE + rb->head - rb->tail) & RB_MASK;
}

bool RB_IsEmpty(const RingBuffer *rb)
{
  return rb->head == rb->tail;
}

bool RB_IsFull(const RingBuffer *rb)
{
  return ((rb->head + 1U) & RB_MASK) == rb->tail;
}
