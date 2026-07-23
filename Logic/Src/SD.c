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

#include <stdbool.h>

FATFS fs;
FIL file;

bool g_file_opened = false;

void SD_Mount(void)
{
  FRESULT res = f_mount(&fs, "0:", 1);

  if (FR_NO_FILESYSTEM == res)
  {
    /* New/blank cards may not contain a valid FAT filesystem yet. */
    static BYTE work[4096];
    res = f_mkfs("0:", FM_FAT | FM_FAT32, 0, work, sizeof(work));
    if (FR_OK == res)
    {
      res = f_mount(&fs, "0:", 1);
    }
  }

  if (FR_OK != res)
  {
    Error_Handler();
  }
}

FRESULT SD_OpenFile(void)
{
  FRESULT res;
  res = f_open(&file, "0:data", FA_OPEN_ALWAYS | FA_WRITE);
  if (FR_OK == res)
  {
    g_file_opened = true;
  }
  return res;
}

FRESULT SD_CloseFile(void)
{
  FRESULT res;
  if (g_file_opened)
  {
    FRESULT res = f_close(&file);
    g_file_opened = false;
  } 
  return res;
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

