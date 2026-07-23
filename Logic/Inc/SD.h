/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SD.h
  * @brief   This file contains all the function prototypes for
  *          the SD.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SD_H__
#define __SD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "fatfs.h"

/** SD卡检测去抖时间（ms） */
#define SD_CARD_DEBOUNCE_MS 30U

/** SD卡是否检测到 */
extern bool g_sd_card_detected;

/** SD卡是否正在初始化 */
extern bool g_sd_card_initializing;

/** SD卡是否初始化 */
extern bool g_sd_card_initialized;

/** SD卡是否挂载 */
extern bool g_sd_card_mounted;

/** SD卡文件是否打开 */
extern bool g_file_opened;

/** SD卡检测中断待处理标志 */
extern volatile bool g_sd_card_detect_irq_pending;

/** SD卡检测中断时间戳 */
extern volatile uint32_t g_sd_card_detect_irq_tick;

/** 检测SD卡 */
bool SD_Detect(void);

/** 挂载SD卡 */
bool SD_Mount(void);

/** 打开文件 */
bool SD_OpenFile(void);

/** 写入文件 */
bool SD_WriteFile(const void* buff, UINT btw, UINT* bw);

/** 关闭文件 */
bool SD_CloseFile(void);

/** 卸载SD卡 */
bool SD_DeMount(void);

/**
 * @brief  获取SD卡剩余空间和总空间
 * 
 * @param[out]  free_bytes: 指向存储剩余空间的指针
 * @param[out]  total_bytes: 指向存储总空间的指针
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-22
 */
void SD_GetSpace(uint64_t* free_bytes, uint64_t* total_bytes);

/** 发送SD卡信息 */
void SD_SendInfo(void);

#ifdef __cplusplus
}
#endif

#endif /* __SD_H__ */

