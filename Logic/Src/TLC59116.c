/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    TLC59116.c
  * @brief   This file provides code for the configuration
  *          of the TLC59116 instances.
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
#include "TLC59116.h"
#include "Utils.h"

/* USER CODE BEGIN 0*/

#define TLC59116_ADDR_1_W     0x60U << 1 //大板上的TLC59116地址
#define TLC59116_ADDR_2_W     0x61U << 1 //小板上的TLC59116地址
#define TLC59116_REG_MODE1    0x00U
#define TLC59116_REG_MODE2    0x01U
#define TLC59116_REG_LEDOUT0  0x14U
#define TLC59116_REG_LEDOUT1  0x15U
#define TLC59116_REG_LEDOUT2  0x16U
#define TLC59116_REG_LEDOUT3  0x17U
#define TLC59116_REG_PWM0     0x02U

void TLC59116_1_Reset(void)
{
  HAL_GPIO_WritePin(TLC59116_1_RESET_GPIO_Port, TLC59116_1_RESET_Pin, GPIO_PIN_RESET);
  Delay_US(1);
  HAL_GPIO_WritePin(TLC59116_1_RESET_GPIO_Port, TLC59116_1_RESET_Pin, GPIO_PIN_SET);
  Delay_US(1);
}

void TLC59116_2_Reset(void)
{
  HAL_GPIO_WritePin(TLC59116_2_RESET_GPIO_Port, TLC59116_2_RESET_Pin, GPIO_PIN_RESET);
  Delay_US(1);
  HAL_GPIO_WritePin(TLC59116_2_RESET_GPIO_Port, TLC59116_2_RESET_Pin, GPIO_PIN_SET);
  Delay_US(1);
}

HAL_StatusTypeDef TLC59116_WriteReg(uint8_t addr7, uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c2, addr7, reg, I2C_MEMADD_SIZE_8BIT, &value, 1U, 100U);
}

HAL_StatusTypeDef TLC59116_InitPwmMode(uint8_t addr7)
{
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_IsDeviceReady(&hi2c2, addr7, 3U, 20U);
  if (status != HAL_OK) { 
    //return status;
  }

  status = TLC59116_WriteReg(addr7, TLC59116_REG_MODE1, 0x00U);
  if (status != HAL_OK)  {
    return status;
  }

  status = TLC59116_WriteReg(addr7, TLC59116_REG_MODE2, 0x00U);
  if (status != HAL_OK)  {
    return status;
  }

  status = TLC59116_WriteReg(addr7, TLC59116_REG_LEDOUT0, 0xAAU);
  if (status != HAL_OK)  {
    return status;
  }

   status = TLC59116_WriteReg(addr7, TLC59116_REG_LEDOUT1, 0xAAU);
  if (status != HAL_OK)  {
    return status;
  }

  status = TLC59116_WriteReg(addr7, TLC59116_REG_LEDOUT2, 0xAAU);
  if (status != HAL_OK)  {
    return status;
  }

  status = TLC59116_WriteReg(addr7, TLC59116_REG_LEDOUT3, 0xAAU);
  if (status != HAL_OK)  {
    return status;
  }

  return HAL_OK;
}

void TLC59116_Init(void)
{
  TLC59116_1_Reset();
  TLC59116_2_Reset();
  TLC59116_InitPwmMode(TLC59116_ADDR_1_W);
  TLC59116_InitPwmMode(TLC59116_ADDR_2_W);
}

HAL_StatusTypeDef TLC59116_SetPwm(uint8_t addr7, uint8_t channel, uint8_t pwm)
{
  return HAL_I2C_Mem_Write(&hi2c2, addr7, (uint16_t)(TLC59116_REG_PWM0 + channel), I2C_MEMADD_SIZE_8BIT, &pwm, 1U, 100U);
}

HAL_StatusTypeDef TLC59116_1_SetPwm(uint8_t channel, uint8_t pwm)
{
  return TLC59116_SetPwm(TLC59116_ADDR_1_W, channel, pwm);
}

HAL_StatusTypeDef TLC59116_2_SetPwm(uint8_t channel, uint8_t pwm)
{
  return TLC59116_SetPwm(TLC59116_ADDR_2_W, channel, pwm);
}

HAL_StatusTypeDef TLC59116_SetAllPwm(uint8_t addr7, uint8_t pwm)
{
  uint16_t reg = 0;
  for (uint16_t i = 0; i < 16U; i++)
  {
    reg = i + TLC59116_REG_PWM0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c2, addr7, reg, I2C_MEMADD_SIZE_8BIT, &pwm, 1U, 100U);
    if (status != HAL_OK)  {
      return status;
    }
  }

  return HAL_OK;
}

/* USER CODE END 0 */
