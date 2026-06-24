/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    batterylevel.c
  * @brief   This file provides code for the acquisition
  *          of batterylevel.
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
#include "batterylevel.h"

uint32_t g_adc3_sample = 0U;

float g_battery_voltage = 0.0f;

uint8_t g_battery_level = 0U;

uint32_t get_adc3_sample(void)
{
  HAL_ADC_Start(&hadc3);
  HAL_ADC_PollForConversion(&hadc3, 100U);
  g_adc3_sample = HAL_ADC_GetValue(&hadc3);
  HAL_ADC_Stop(&hadc3);

  return g_adc3_sample;
}

float get_battery_voltage(void)
{
  // TODO: 从ADC3配置中获取分辨率
  g_battery_voltage = (float)g_adc3_sample / 65535.0f * 3.27f;

  return g_battery_voltage;
}

uint8_t get_battery_level(void)
{
  // 标称 7.4V，满电 8.4V，放尽 ≈ 6.6V
  float volt = g_battery_voltage * 2.8f / 2.0f;

  if (volt >= 4.20f) g_battery_level = 100;
  if (volt >= 4.15f) g_battery_level = 95;
  if (volt >= 4.10f) g_battery_level = 90;
  if (volt >= 4.05f) g_battery_level = 80;
  if (volt >= 4.00f) g_battery_level = 70;
  if (volt >= 3.95f) g_battery_level = 60;
  if (volt >= 3.90f) g_battery_level = 50;
  if (volt >= 3.85f) g_battery_level = 40;
  if (volt >= 3.80f) g_battery_level = 30;
  if (volt >= 3.75f) g_battery_level = 20;
  if (volt >= 3.70f) g_battery_level = 15;
  if (volt >= 3.65f) g_battery_level = 10;
  if (volt >= 3.55f) g_battery_level = 5;
  if (volt >= 3.30f) g_battery_level = 1;

  return g_battery_level;
}

