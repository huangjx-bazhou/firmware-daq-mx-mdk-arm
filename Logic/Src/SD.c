/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SD.c
  * @brief   This file provides code for the SD Card.
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

#include "SD.h"
#include <string.h>
#include <usart.h>

FATFS fs;
FIL file;

// SD卡是否检测到
bool g_sd_card_detected = false;

// SD卡是否正在初始化
bool g_sd_card_initializing = false;

// SD卡是否初始化
bool g_sd_card_initialized = false;

// SD卡是否挂载
bool g_sd_card_mounted = false;

// SD卡文件是否打开
bool g_file_opened = false;

// SD卡检测中断待处理标志
volatile bool g_sd_card_detect_irq_pending = false;

// SD卡检测中断时间戳
volatile uint32_t g_sd_card_detect_irq_tick = 0U;

bool SD_Detect(void)
{
  g_sd_card_detected = (GPIO_PIN_RESET == HAL_GPIO_ReadPin(Card_Detect_GPIO_Port, Card_Detect_Pin));
  return g_sd_card_detected;
}

bool SD_Mount(void)
{
  g_sd_card_mounted = g_sd_card_mounted ? g_sd_card_mounted : (g_sd_card_initialized ? (FR_OK == f_mount(&fs, "0:", 1)) : g_sd_card_initialized);
  return g_sd_card_mounted;
}

bool SD_OpenFile(void)
{
  /// TODO: 新打开文件 要从RTC获取时间戳，拼接成文件名
  g_file_opened = g_file_opened ? g_file_opened : (g_sd_card_mounted ? (FR_OK == f_open(&file, "0:data", FA_OPEN_ALWAYS | FA_WRITE)) : g_sd_card_mounted);
  return g_file_opened;
}

bool SD_WriteFile(const void* buff, UINT btw, UINT* bw)
{
  if (!g_file_opened)
  {
    return g_file_opened;
  }
  f_lseek(&file, f_size(&file));
  return FR_OK == f_write(&file, buff, btw, bw);
}

bool SD_CloseFile(void)
{
  g_file_opened = g_file_opened ? (FR_OK == f_close(&file) ? false : true) : g_file_opened;
  return !g_file_opened;
}

bool SD_DeMount(void)
{
  g_sd_card_mounted = g_sd_card_mounted ? (FR_OK == f_mount(NULL, "0:", 1) ? false : true) : g_sd_card_mounted;
  return !g_sd_card_mounted;
}

void SD_GetSpace(uint64_t* free_bytes, uint64_t* total_bytes)
{
	FATFS* pfs = &fs;
  DWORD fre_clust;
	
  if (FR_OK != f_getfree("0:", &fre_clust, &pfs))
	{
		return;
  }
	
  DWORD tot_sect;
	DWORD fre_sect;

  /* 计算扇区数（每簇扇区数 = fs->csize，每扇区默认 512 字节） */
  tot_sect = (fs.n_fatent - 2) * fs.csize;
  fre_sect = fre_clust * fs.csize;

  *total_bytes = (uint64_t)tot_sect * 512;
  *free_bytes  = (uint64_t)fre_sect * 512;
}

void SD_SendInfo(void)
{
  uint64_t free_bytes = 0U;   /*  剩余空间(字节) */
  uint64_t total_bytes = 0U;  /*  总空间(字节)   */

  if (g_sd_card_mounted)
  {
    SD_GetSpace(&free_bytes, &total_bytes);
  }

  /* SD卡信息缓存区 */
  uint8_t sd_info[449];
  memset(sd_info, 0, sizeof(sd_info));
  sd_info[0] = 0x84;
  sd_info[1] = 0x6F;
  sd_info[2] = 0x0B;
  sd_info[3] = 0x01;

  uint8_t offset = 4U;

  uint8_t status = 0U;
  if (!g_sd_card_detected)
  {
    status = 0x01;
  }
  else if (!g_sd_card_initialized)
  {
    status = 0x02;
  }
  else if (!g_sd_card_mounted)
  {
    status = 0x03;
  }
  else
  {
    status = 0x00;
  }

  /* 获取剩余空间和总空间，获取成功才发送到上位机，获取失败发送到上位机的是0 */
  memcpy(sd_info + offset, &status, sizeof(status));
  offset += sizeof(status);
  memcpy(sd_info + offset, &free_bytes, sizeof(free_bytes));
  offset += sizeof(free_bytes);
  memcpy(sd_info + offset, &total_bytes, sizeof(total_bytes));
  HAL_UART_Transmit(&huart3, sd_info, sizeof(sd_info), 100);
}

