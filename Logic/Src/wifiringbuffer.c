/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    wifiringbuffer.c
  * @brief   This file provides code for the wifiringbuffer.
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
#include "wifiringbuffer.h"

// 定义环形缓冲区大小
#define WIFI_RB_SIZE 64U

// 定义环形缓冲区
volatile uint8_t wifi_rb_buf[WIFI_RB_SIZE];

// 定义环形缓冲区头指针
volatile uint8_t wifi_rb_head = 0U;

// 定义环形缓冲区尾指针
volatile uint8_t wifi_rb_tail = 0U;

void wifi_rb_write(uint8_t data)
{
  uint8_t next = (wifi_rb_head + 1U) % WIFI_RB_SIZE;
  if (next != wifi_rb_tail)
  {
    wifi_rb_buf[wifi_rb_head] = data;
    wifi_rb_head = next;
  }
}

uint8_t wifi_rb_read(uint8_t *data)
{
  if (wifi_rb_head == wifi_rb_tail)
  {
    return 0U;
  }
  *data = wifi_rb_buf[wifi_rb_tail];
  wifi_rb_tail = (wifi_rb_tail + 1U) % WIFI_RB_SIZE;
  return 1U;
}

uint8_t wifi_rb_available(void)
{
  return (WIFI_RB_SIZE + wifi_rb_head - wifi_rb_tail) % WIFI_RB_SIZE;
}
