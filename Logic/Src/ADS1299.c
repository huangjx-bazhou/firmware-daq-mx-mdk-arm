/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ADS1299.c
  * @brief   This file provides code for the configuration
  *          of the ADS1299 ADC.
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

#include "ADS1299.h"
#include "Utils.h"

/* System Commands */
#define ADS1299_CMD_WAKEUP  0x02U  /* Wake-up from standby mode */
#define ADS1299_CMD_STANDBY 0x04U  /* Enter standby mode */
#define ADS1299_CMD_RESET   0x06U  /* Reset the device */
#define ADS1299_CMD_START   0x08U  /* Start and restart (synchronize) conversions */
#define ADS1299_CMD_STOP    0x0AU  /* Stop conversion */

/* Data Read Commands */
#define ADS1299_CMD_RDATAC 0x10U /* Enable Read Data Continuous mode.This mode is the default mode at power-up. */
#define ADS1299_CMD_SDATAC 0x11U /* Stop Read Data Continuously mode */
#define ADS1299_CMD_RDATA  0x12U /* Read data by command; supports multiple read back. */

/* Configuration Registers */
#define ADS1299_REG_CONFIG1 0x01U
#define ADS1299_REG_CONFIG2 0x02U
#define ADS1299_REG_CONFIG3 0x03U
#define ADS1299_REG_CONFIG4 0x17U

uint8_t ads_tx_buffer[ADS1299_DATA_SIZE];
uint8_t ads_1_rx_buffer[ADS1299_DATA_SIZE];
uint8_t ads_2_rx_buffer[ADS1299_DATA_SIZE];
uint8_t ads_1_origin_rx_buffer[ADS1299_DATA_SIZE];
uint8_t ads_2_origin_rx_buffer[ADS1299_DATA_SIZE];
int32_t ads_1_origin[ADS1299_MAX_CHANNEL];
int32_t ads_2_origin[ADS1299_MAX_CHANNEL];

void ADS1299_1_Reset(void)
{
  /// TODO: 延时时间待确认
  HAL_GPIO_WritePin(ADS1299_1_RESET_GPIO_Port, ADS1299_1_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(ADS1299_1_RESET_GPIO_Port, ADS1299_1_RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(5);
}

void ADS1299_2_Reset(void)
{
  /// TODO: 延时时间待确认
  HAL_GPIO_WritePin(ADS1299_2_RESET_GPIO_Port, ADS1299_2_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(ADS1299_2_RESET_GPIO_Port, ADS1299_2_RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(5);
}

void ADS1299_1_CS_Low(void)
{
  HAL_GPIO_WritePin(ADS1299_1_CS_GPIO_Port, ADS1299_1_CS_Pin, GPIO_PIN_RESET);
}

void ADS1299_1_CS_High(void)
{
  HAL_GPIO_WritePin(ADS1299_1_CS_GPIO_Port, ADS1299_1_CS_Pin, GPIO_PIN_SET);
}

void ADS1299_2_CS_Low(void)
{
  HAL_GPIO_WritePin(ADS1299_2_CS_GPIO_Port, ADS1299_2_CS_Pin, GPIO_PIN_RESET);
}

void ADS1299_2_CS_High(void)
{
  HAL_GPIO_WritePin(ADS1299_2_CS_GPIO_Port, ADS1299_2_CS_Pin, GPIO_PIN_SET);
}

void ADS1299_Start_ByCmd(void)
{
  ADS1299_SendCmd(ADS1299_CMD_START);
}

void ADS1299_Stop_ByCmd(void)
{
  ADS1299_SendCmd(ADS1299_CMD_STOP);
}

void ADS1299_Start(void)
{
  HAL_GPIO_WritePin(ADS1299_START_GPIO_Port, ADS1299_START_Pin, GPIO_PIN_SET);
}

void ADS1299_Stop(void)
{
  HAL_GPIO_WritePin(ADS1299_START_GPIO_Port, ADS1299_START_Pin, GPIO_PIN_RESET);
}

void ADS1299_SendCmd(uint8_t cmd)
{
  /// TODO: 超时时间待确认
  HAL_SPI_Transmit(&hspi1, &cmd, 1U, HAL_MAX_DELAY);

  /// TODO: 延时时间待确认
  Delay_US(2);  // 等待SDECODE时间
}

void ADS1299_WriteReg(uint8_t reg, uint8_t value)
{
  // 组装命令
  uint8_t tx[3];
  tx[0] = (uint8_t)(0x40U | (reg & 0x1FU));
  tx[1] = 0x00U;
  tx[2] = value;

  ADS1299_SendCmd(tx[0]);
  ADS1299_SendCmd(tx[1]);
  ADS1299_SendCmd(tx[2]);
}

uint8_t ADS1299_ReadReg(uint8_t reg)
{
  uint8_t tx[3] = {0x20 | (reg & 0x1F), 0x00, 0x00};
  uint8_t rx[3];
  HAL_SPI_TransmitReceive(&hspi1, tx, rx, 3, HAL_MAX_DELAY);
  return rx[2];
}

void ADS1299_1_Init(void)
{
  // 复位
  ADS1299_1_Reset();

  // 等待POR时间
  HAL_Delay(200);

  // 拉低CS
  ADS1299_1_CS_Low();

  // 退出连续读模式
  ADS1299_SendCmd(ADS1299_CMD_SDATAC);

  // 配置寄存器
  ADS1299_WriteReg(ADS1299_REG_CONFIG3, 0xE0U);

  // 配置寄存器-16KSPS
  ADS1299_WriteReg(ADS1299_REG_CONFIG1, 0xF0U);

  // 配置寄存器
  ADS1299_WriteReg(ADS1299_REG_CONFIG2, 0xC0U);

  // 配置寄存器-单次转换
  ADS1299_WriteReg(ADS1299_REG_CONFIG4, 0x08U);

  for (uint8_t ch = 0; ch < 8; ch++)
  {
    ADS1299_WriteReg(0x05 + ch, 0x00);
  }

  ADS1299_SendCmd(ADS1299_CMD_RDATAC);

  ADS1299_1_CS_High();

  HAL_Delay(1);
}

void ADS1299_2_Init(void)
{
  // 复位
  ADS1299_2_Reset();

  // 等待tPOR时间
  HAL_Delay(200);

  // 拉低CS
  ADS1299_2_CS_Low();

  // 退出连续读模式
  ADS1299_SendCmd(ADS1299_CMD_SDATAC);

  ADS1299_WriteReg(ADS1299_REG_CONFIG3, 0xE0U);

  ADS1299_WriteReg(ADS1299_REG_CONFIG1, 0xD0U);

  ADS1299_WriteReg(ADS1299_REG_CONFIG2, 0xC0U);

  ADS1299_WriteReg(ADS1299_REG_CONFIG4, 0x08U);

  for (uint8_t ch = 0; ch < 8; ch++)
  {
    ADS1299_WriteReg(0x05 + ch, 0x00);
  }

  ADS1299_SendCmd(ADS1299_CMD_RDATAC);

  ADS1299_2_CS_High();

  HAL_Delay(1);
}

void ADS1299_1_Origin_Read(void)
{
  ADS1299_1_CS_Low();

  // 开启采集
  ADS1299_Start();

  // 等待PB0为低电平
  while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) != GPIO_PIN_RESET)
  {
  }

  // 读取ADS12991数据寄存器
  HAL_SPI_TransmitReceive(&hspi1, ads_tx_buffer, ads_1_origin_rx_buffer, 27, 100U);

  for (uint8_t i = 0; i < 8; i++)
  {
    ads_1_origin[i] =  Sign_Extend_24_to_32(&ads_1_origin_rx_buffer[3 + i * 3]);
  }

  // 关闭采集
  ADS1299_Stop();

  ADS1299_1_CS_High();
}

void ADS1299_2_Origin_Read(void)
{
  ADS1299_2_CS_Low();

  ADS1299_Start();

  // 等待PA0为低电平
  while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) != GPIO_PIN_RESET)
  {
  }

  // 读取ADS12992数据寄存器
  HAL_SPI_TransmitReceive(&hspi1, ads_tx_buffer, ads_2_origin_rx_buffer, 27, 100U);

  for (uint8_t i = 0; i < 8; i++)
  {
    ads_2_origin[i] =  Sign_Extend_24_to_32(&ads_2_origin_rx_buffer[3 + i * 3]);
  }

  // 关闭采集
  ADS1299_Stop();

  ADS1299_2_CS_High();
}

/* USER CODE END 0 */

