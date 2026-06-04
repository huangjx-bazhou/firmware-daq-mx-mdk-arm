/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c2;

/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00707CBB;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
    PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);

  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

#define TLC59116_REG_MODE1    0x00U
#define TLC59116_REG_MODE2    0x01U
#define TLC59116_REG_PWM0     0x02U
#define TLC59116_REG_LEDOUT0  0x14U
#define TLC59116_REG_LEDOUT1  0x15U
#define TLC59116_REG_LEDOUT2  0x16U
#define TLC59116_REG_LEDOUT3  0x17U

static HAL_StatusTypeDef TLC59116_WriteReg(uint8_t addr7, uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c2, (uint16_t)(addr7 << 1), reg, I2C_MEMADD_SIZE_8BIT, &value, 1U, 100U);
}

HAL_StatusTypeDef TLC59116_InitPwmMode(uint8_t addr7)
{
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(addr7 << 1), 3U, 20U);
  if (status != HAL_OK)
  {
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

HAL_StatusTypeDef TLC59116_SetAllPwm(uint8_t addr7, uint8_t pwm)
{
  uint16_t reg = 0;
  for (uint16_t i = 0; i < 16U; i++)
  {
    reg = i + TLC59116_REG_PWM0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c2, (uint16_t)(addr7 << 1), reg, I2C_MEMADD_SIZE_8BIT, &pwm, 1U, 100U);
    if (status != HAL_OK)  {
      return status;
    }
  }

  return HAL_OK;
}

HAL_StatusTypeDef TLC59116_SetPwm(uint8_t addr7, uint8_t channel, uint8_t pwm)
{
  // 检查地址，通道，PWM值的合法性
  // 从设备地址必须在0x60到0x6F之间(7位地址模式)
  if (addr7 < 0x60U || addr7 > 0x6F)
  {
    return HAL_ERROR;
  }

  // 通道号必须在0到15之间
  if (channel > 15U)
  {
    return HAL_ERROR;
  }

  return HAL_I2C_Mem_Write(&hi2c2, (uint16_t)(addr7 << 1), (uint16_t)(TLC59116_REG_PWM0 + channel), I2C_MEMADD_SIZE_8BIT, &pwm, 1U, 100U);
}

/* USER CODE END 1 */

