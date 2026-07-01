/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ads1299.h
  * @brief   This file contains all the function prototypes for
  *          the ads1299.c file
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
#ifndef __ADS1299_H__
#define __ADS1299_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "spi.h"

/* USER CODE BEGIN Prototypes */

extern uint8_t spi_tx_buffer[27];
extern uint8_t ads_1_origin_rx_buffer[27];
extern uint8_t ads_2_origin_rx_buffer[27];
extern int32_t ads_1_origin[8];
extern int32_t ads_2_origin[8];

/**
 * @brief   复位第1个ADS1299芯片
 *
 * 将第1个ADS1299芯片的RESET引脚拉低，等待1ms后再拉高，再等待5ms，完成复位
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
 * 第2个ADS1299芯片的CS引脚连接到PD7
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-08
 */
void ADS1299_2_CS_Low(void);

/**
 * @brief   将第2个ADS1299芯片的CS引脚拉高，禁用此芯片的SPI通信
 *
 * 第2个ADS1299芯片的CS引脚连接到PD7
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-08
 */
void ADS1299_2_CS_High(void);

/**
 * @brief 开始ADS1299芯片的采集，通过SPI发送命令字节
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-22
 */
void ADS1299_Start(void);

/**
 * @brief 停止ADS1299芯片的采集，通过SPI发送命令字节
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-22
 */
void ADS1299_Stop(void);

/**
 * @brief   SPI发送命令字节
 * @param   cmd 命令字节
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-12
 */
void ADS1299_SendCmd(uint8_t cmd);

/**
 * @brief   SPI写入单个寄存器
 * @param   reg   寄存器地址 (0x00-0x17)
 * @param   value 写入值
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-12
 */
void ADS1299_WriteReg(uint8_t reg, uint8_t value);

/**
 * @brief   SPI读取单个寄存器
 * @param   reg 寄存器地址 (0x00-0x17)
 * @return  寄存器值
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-12
 */
uint8_t ADS1299_ReadReg(uint8_t reg);

/**
 * @brief   初始化第1个ADS1299芯片
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-12
 */
void ADS1299_1_Init(void);

/**
 * @brief   初始化第2个ADS1299芯片
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-12
 */
void ADS1299_2_Init(void);

/**
 * @brief   将不开LED读取的数据定义为原点数据，读取第1个ADS1299芯片的原点数据
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-22
 */
void ADS1299_1_Origin_Read(void);

/**
 * @brief   将不开LED读取的数据定义为原点数据，读取第1个ADS1299芯片的原点数据
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-06-22
 */
void ADS1299_2_Origin_Read(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ ADS1299_H__ */

