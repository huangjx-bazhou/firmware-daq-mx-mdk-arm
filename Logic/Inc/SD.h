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

/**
 * @brief  检测SD卡
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-23
 */
void SD_Detect(void);

/**
 * @brief  挂载SD卡
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-16
 */
void SD_Mount(void);

/**
 * @brief  打开文件
 * 
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-16
 */
void SD_OpenFile(void);

/**
 * @brief  关闭文件
 * 
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-16
 */
void SD_CloseFile(void);

/**
 * @brief  写入文件
 * 
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-16
 */
FRESULT SD_WriteFile(const void* buff, UINT btw, UINT* bw);

/**
 * @brief  获取SD卡剩余空间和总空间
 * 
 * @param[out]  free_bytes: 指向存储剩余空间的指针
 * @param[out]  total_bytes: 指向存储总空间的指针
 * @return FRESULT: 返回FRESULT类型的结果
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-22
 */
FRESULT SD_GetSpace(uint64_t* free_bytes, uint64_t* total_bytes);

/**
 * @brief  卸载SD卡
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-16
 */
void SD_DeMount(void);

#ifdef __cplusplus
}
#endif

#endif /* __SD_H__ */

