/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/********************************************************
 * ADS1299芯片相关函数
 ********************************************************/

/**
 * @brief   复位第1个ADS1299芯片
 *
 * 将第1个ADS1299芯片的RESET引脚拉低，等待1ms后再拉高，再等待5ms，完成复位
 * 第1个ADS1299芯片的RESET引脚连接到PB7
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-05
 */
void ADS1299_1_Reset(void);

/**
 * @brief   复位第2个ADS1299芯片
 *
 * 将第2个ADS1299芯片的RESET引脚拉低，等待1ms后再拉高，再等待5ms，完成复位
 * 第2个ADS1299芯片的RESET引脚连接到PA5
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-05
 */
void ADS1299_2_Reset(void);

/**
 * @brief   将第1个ADS1299芯片的CS引脚拉低，使能此芯片的SPI通信
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-08
 */
void ADS1299_1_CS_Low(void);

/**
 * @brief   将第1个ADS1299芯片的CS引脚拉高，禁用此芯片的SPI通信
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-08
 */
void ADS1299_1_CS_High(void);

/**
 * @brief   将第2个ADS1299芯片的CS引脚拉低，使能此芯片的SPI通信
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-08
 */
void ADS1299_2_CS_Low(void);

/**
 * @brief   将第2个ADS1299芯片的CS引脚拉高，禁用此芯片的SPI通信
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-08
 */
void ADS1299_2_CS_High(void);

/**
 * @brief 开始2个ADS1299芯片的采集
 *
 * 两个ADS1299的START引脚拉高，都与MCU的PE8引脚连接
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-05
 */
void ADS1299_Start(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

