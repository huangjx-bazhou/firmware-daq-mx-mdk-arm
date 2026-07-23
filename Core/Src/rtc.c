/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */

/** TODO: AI生成待优化验证 */

#define RTC_BKPORL_VALUE 0x1A1B

#define OS_YEAR ((((__DATE__[7] - '0') * 10 + (__DATE__[8] - '0')) * 10 + (__DATE__[9] - '0')) * 10 + (__DATE__[10] - '0'))

#define OS_MONTH (__DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? 1 : 6) \
														: __DATE__[2] == 'b' ? 2 \
														: __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 3 : 4) \
														: __DATE__[2] == 'y' ? 5 \
														: __DATE__[2] == 'l' ? 7 \
														: __DATE__[2] == 'g' ? 8 \
														: __DATE__[2] == 'p' ? 9 \
														: __DATE__[2] == 't' ? 10 \
														: __DATE__[2] == 'v' ? 11 : 12)
														
#define OS_DAY ((__DATE__[4] == ' ' ? 0 : __DATE__[4] - '0') * 10 + (__DATE__[5] - '0'))

#define OS_HOUR ((__TIME__[0] - '0') * 10 + (__TIME__[1] - '0'))

#define OS_MINUTE ((__TIME__[3] - '0') * 10 + (__TIME__[4] - '0'))

#define OS_SECOND ((__TIME__[6] - '0') * 10 + (__TIME__[7] - '0'))

uint32_t BKUPReadDRx;

void System_Time_Init(void);

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
	
	BKUPReadDRx = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
	
	if ((BKUPReadDRx & 0xFFFF) == RTC_BKPORL_VALUE)
	{
		return;
	}
	
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
	System_Time_Init();
  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void System_Time_Init(void)
{
	int32_t year = OS_YEAR;
	int32_t month = OS_MONTH;
	int32_t date = OS_DAY;
	
	int32_t week = 0;
	int32_t weekBuff = 0;
	
	// 基姆拉尔森公式
	if (1 == month || 2 == month)
	{
		month += 12;
		year--;
	}
	
	weekBuff = (date + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
	
	switch (weekBuff)
	{
		case 0: week = RTC_WEEKDAY_MONDAY; break;
		case 1: week = RTC_WEEKDAY_TUESDAY; break;
		case 2: week = RTC_WEEKDAY_WEDNESDAY; break;
		case 3: week = RTC_WEEKDAY_THURSDAY; break;
		case 4: week = RTC_WEEKDAY_FRIDAY; break;
		case 5: week = RTC_WEEKDAY_SATURDAY; break;
		case 6: week = RTC_WEEKDAY_SUNDAY; break;	
	}
	
	RTC_DateTypeDef systemdate;
	RTC_TimeTypeDef systemtime;
	
	systemtime.Hours = OS_HOUR;
	systemtime.Minutes = OS_MINUTE;
	systemtime.Seconds = OS_SECOND;
	systemtime.SubSeconds = 0;
	systemtime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	systemtime.StoreOperation = RTC_STOREOPERATION_RESET;
	
	if (HAL_RTC_SetTime(&hrtc, &systemtime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
	
	systemdate.WeekDay = week;
	systemdate.Month = OS_MONTH;
	systemdate.Date = OS_DAY;
	systemdate.Year = OS_YEAR - 2000;
	if (HAL_RTC_SetDate(&hrtc, &systemdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
	
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, (BKUPReadDRx & 0xFFFF0000) | RTC_BKPORL_VALUE);
}

/* USER CODE END 1 */

