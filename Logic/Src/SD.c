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

void SD_Detect(void)
{
  g_sd_card_detected = GPIO_PIN_RESET == HAL_GPIO_ReadPin(Card_Detect_GPIO_Port, Card_Detect_Pin);
}

void SD_Mount(void)
{
  g_sd_card_mounted = g_sd_card_detected ? (FR_OK == f_mount(&fs, "0:", 1)) : false;
}

void SD_OpenFile(void)
{
  g_file_opened = g_sd_card_mounted ? (FR_OK == f_open(&file, "0:data", FA_OPEN_ALWAYS | FA_WRITE)) : false;
}

void SD_CloseFile(void)
{
  g_file_opened = g_file_opened ? (FR_OK == f_close(&file) ? false : true) : false;
}

FRESULT SD_WriteFile(const void* buff, UINT btw, UINT* bw)
{
  if (!g_file_opened)
  {
    return FR_INVALID_OBJECT;
  }
  f_lseek(&file, f_size(&file));
  FRESULT res = f_write(&file, buff, btw, bw);
  return res;
}

void SD_DeMount(void)
{
   f_mount(NULL, "0:", 1);
}

FRESULT SD_GetSpace(uint64_t* free_bytes, uint64_t* total_bytes)
{
  DWORD fre_clust;
  DWORD tot_sect, fre_sect;
  FRESULT res;

	FATFS* pfs = &fs;
	
  res = f_getfree("0:", &fre_clust, &pfs);
  if (res != FR_OK) {
      return res;
  }

  /* 计算扇区数（每簇扇区数 = fs->csize，每扇区默认 512 字节） */
  tot_sect = (fs.n_fatent - 2) * fs.csize;
  fre_sect = fre_clust * fs.csize;

  *total_bytes = (uint64_t)tot_sect * 512;
  *free_bytes  = (uint64_t)fre_sect * 512;
	
	return FR_OK;
}

