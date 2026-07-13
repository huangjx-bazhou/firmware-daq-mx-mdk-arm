/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    BatteryLevel.c
  * @brief   This file provides code for the acquisition of BatteryLevel.
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

#include "BatteryLevel.h"
#include "adc.h"

uint32_t g_adc3_sample = 0U;

float g_battery_voltage = 0.0f;

uint8_t g_battery_level = 0U;

uint32_t Get_ADC3_Sample(void)
{
  HAL_ADC_Start(&hadc3);
  HAL_ADC_PollForConversion(&hadc3, 100U);
  g_adc3_sample = HAL_ADC_GetValue(&hadc3);
  HAL_ADC_Stop(&hadc3);
  return g_adc3_sample;
}

float Get_Battery_Voltage(void)
{
  Get_ADC3_Sample();
  /* TODO: 从ADC3配置中获取分辨率 */
  g_battery_voltage = (float)g_adc3_sample / 65535.0f * 3.27f;
  return g_battery_voltage;
}

uint8_t Get_Battery_Level(void)
{
  Get_Battery_Voltage();

  float volt = g_battery_voltage * 2.8f / 2.0f;

  if (volt > 4.20f) volt = 4.20f;
  else if (volt < 3.30f) volt = 3.30f;

  /* TODO: 根据电池官方手册，计算电量值，没有找到相关资料  */
  if (volt > 4.00f) g_battery_level = 100.0f * volt - 320.0f;
  else if (volt > 3.75f) g_battery_level = 200.0f * volt - 720.0f;
  else if (volt > 3.60f) g_battery_level = 133.3333f * volt - 470.0f;
  else if (volt > 3.40f) g_battery_level = 25.0f * volt - 80.0f;
  else g_battery_level = 50.0f * volt - 165.0f;

  return g_battery_level;
}
