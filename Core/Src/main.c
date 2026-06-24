/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "ads1299.h"
#include "tlc59116.h"
#include "ringbuffer.h"
#include "parsecmd.h"
#include "batterylevel.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CALIBRATE_FREQUENCY 15000U // 校准频率每15000次校准一次
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// 定时器6中断标志(0: 未中断, 1: 中断)
static volatile uint16_t g_tim6_ready = 0U;

// 定时器6中断计数
static uint16_t g_tim6_ready_count = 0U;

// USART3接收缓冲区(WIFI)
static uint8_t g_usart3_rx_byte = 0U;

// SPI ADS1299
uint8_t spi_ads_1_rx_buffer[27];  // 第一个ADS1299接收缓冲区
uint8_t spi_ads_2_rx_buffer[27];  // 第二个ADS1299接收缓冲区

uint8_t spi_ads_1_origin_rx_buffer[27];
uint8_t spi_ads_2_origin_rx_buffer[27];

// 逐字节读取并验证头字节
static uint8_t cmd_state = 0;  // 0:等待第一个头字节, 1:等待第二个头字节, 2:读取数据
static uint8_t cmd_buf[CMD_PACKET_SIZE];  // CMD_PACKET_SIZE已包括2字节头
static uint16_t cmd_len = 0;

// ADS1299数据
static int32_t g_ads_data[LED_COUNT * RECVIVER_COUNT * CHANNEL_COUNT_PER_LED];

// 数据个数
static uint16_t g_ads_data_count = 0U;

// 采集索引
static uint8_t g_sample_idx = 0U;

// 数据包序号
static uint32_t g_packet_num = 0U;

// 数据包,需要发送到上位机
static uint8_t g_packet[8192];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/**
  * @brief  初始化DWT计时器
  */
static void DWT_Init(void);

/**
  * @brief  延时指定微秒数
  * @param us 微秒数
  */
void delay_us(uint32_t us);

/**
  * @brief  检查指定位是否为1
  * @param value 要检查的8位值
  * @param bit 要检查的位（0-7）
  * @return uint8_t 位为1则返回1，否则返回0
  */
uint8_t is_bit_set(uint8_t value, uint8_t bit);

/**
  * @brief  根据采样率配置定时器6
  * @param rate 采样率（Hz）
  * @param new_psc 新的预分频器值（0-65535）
  * @param new_arr 新的自动重载值（0-65535）
  */
static void configure_tim6_for_sample_rate(uint32_t rate, uint16_t* new_psc, uint16_t* new_arr);

static int32_t ads1299_24_to_32(const uint8_t raw[3])
{
    return ((int32_t)(
        ((uint32_t)raw[0] << 24) |
        ((uint32_t)raw[1] << 16) |
        ((uint32_t)raw[2] <<  8)
    )) >> 8;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */

  DWT_Init();

  HAL_UART_Receive_IT(&huart3, &g_usart3_rx_byte, 1U);

  HAL_TIM_Base_Start_IT(&htim6);

  // 初始化所有(2个)TLC59116为PWM模式，设置所有通道的PWM值为0
  TLC59116_Init();
  // 初始化第1个ADS1299芯片
  ADS1299_1_Init();
  // 初始化第2个ADS1299芯片
  ADS1299_2_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // 1.处理WIFI串口命令
    if (rb_available() > 0)
    {
      uint8_t byte;
      rb_read(&byte);

      switch (cmd_state)
      {
        case 0:
          if (byte == 0x84)  // 第一个头字节
          {
            cmd_buf[0] = byte;
            cmd_state = 1;
          }
          break;

        case 1:
          if (byte == 0x6F)  // 第二个头字节
          {
            cmd_buf[1] = byte;
            cmd_state = 2;
            cmd_len = 2;  // 已写入2字节头
          }
          else
          {
            cmd_state = 0;  // 头不匹配，重新开始
          }
          break;

        case 2:
          cmd_buf[cmd_len++] = byte;
          if (cmd_len >= CMD_PACKET_SIZE)
          {
            parse_cmd(cmd_buf);  // 传递完整数据包（包括头）
            HAL_UART_Transmit(&huart3, cmd_buf, cmd_len, HAL_MAX_DELAY);
            cmd_state = 0;  // 解析完成，重新等待头
          }
          break;
      }
    }

    // 2. 处理离线存储标志改变标志
    if (1 == g_storage_flag_changed)
    {
      g_storage_flag_changed = 0;
    }

    // 3. 处理采样率改变标志
    if (1 == g_sample_rate_changed)
    {
      g_sample_rate_changed = 0;

      // 新的预分频器值
      uint16_t new_psc = 0U;
      // 新的自动重载值
      uint16_t new_arr = 0U;

      // 配置定时器6的采样率
      configure_tim6_for_sample_rate(g_sample_rate, &new_psc, &new_arr);

      // 采样率改变，更新定时器6的周期
      __HAL_TIM_SET_PRESCALER(&htim6, new_psc);
      __HAL_TIM_SET_AUTORELOAD(&htim6, new_arr);
      HAL_TIM_GenerateEvent(&htim6, TIM_EVENTSOURCE_UPDATE);
    }

    // 4. 处理开始采样标志改变标志
    if (1 == g_start_flag_changed)
    {
      g_start_flag_changed = 0;

      if (CMD_ENABLE_FLAG == g_start_flag)
      {
        // 当开始采样时，读取一次没有开LED时的原始数据
        //ADS1299_1_Origin_Read();
        //ADS1299_1_Origin_Read();
      }
      else
      {
        g_packet_num = 0;
        g_tim6_ready_count = 0;
      }
    }

    // 5.定时器6中断处理
    if (1 == g_tim6_ready && CMD_ENABLE_FLAG == g_start_flag)
    {
      g_tim6_ready = 0;
      g_tim6_ready_count++;

      if (g_tim6_ready_count >= CALIBRATE_FREQUENCY)
      {
        g_tim6_ready_count = 0;
        //ADS1299_1_Origin_Read();
        //ADS1299_1_Origin_Read();
      }

      // 循环开启TLC59116-1的16个通道
      for (uint8_t ch = 0; ch < LED_COUNT; ch++)
      {
        // 第几个LED
        uint8_t led = ch / 2;

        uint8_t ads1 =  g_channel_mask[led * 2];

        uint8_t ads2 =  g_channel_mask[led * 2 + 1];

        // 如果没有建立通道，跳过开启LED
        if (ads1 == 0 && ads2 == 0)
        {
          continue;
        }

        // 开启LED
        TLC59116_1_SetPwm(ch, g_led_brightness[led]);

        // 延时5us
        delay_us(25);

        // led与ads1建立了通道
        if (ads1 != 0)
        {
          // 选择第一个ADS1299芯片
          ADS1299_1_CS_Low();

          // 开启采集
          ADS1299_Start();

          // 等待PB0为低电平
          while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) != GPIO_PIN_RESET)
          {
          }

          // 读取ADS12991数据寄存器
          HAL_SPI_TransmitReceive(&hspi1, spi_tx_buffer, spi_ads_1_rx_buffer, 27, 100U);

          // 关闭采集
          ADS1299_Stop();

          ADS1299_1_CS_High();

          for (uint8_t i = 0; i < 8; i++)
          {
            uint8_t set = is_bit_set(ads1, i);
            if (1 == set)
            {
              int32_t value =  ads1299_24_to_32(&spi_ads_1_rx_buffer[3 + i * 3]);
              g_ads_data[g_ads_data_count++] = value - ads_1_origin[i];
            }
          }
        }

        // led与ads2建立了通道
        if (ads2 != 0)
        {
          // 选择第一个ADS1299芯片
          ADS1299_2_CS_Low();

          ADS1299_Start();

          // 等待PA0为低电平
          while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) != GPIO_PIN_RESET)
          {
          }

          // 读取ADS12992数据寄存器
          HAL_SPI_TransmitReceive(&hspi1, spi_tx_buffer, spi_ads_2_rx_buffer, 27, 100U);

          // 关闭采集
          ADS1299_Stop();

          ADS1299_2_CS_High();

          for (uint8_t i = 0; i < 8; i++)
          {
            uint8_t set = is_bit_set(ads2, i);
            if (1 == set)
            {
              int32_t value =  ads1299_24_to_32(&spi_ads_2_rx_buffer[3 + i * 3]);
              g_ads_data[g_ads_data_count++] = value - ads_2_origin[i];
            }
          }
        }

        // 关闭LED
        TLC59116_1_SetPwm(ch, 0);
      }

      // 循环开启TLC59116-2的16个通道
      for (uint8_t ch = 0; ch < LED_COUNT; ch++)
      {
        // 第几个LED
        uint8_t led = ch / 2;

        uint8_t ads1 =  g_channel_mask[16 + led * 2];

        uint8_t ads2 =  g_channel_mask[16 + led * 2 + 1];

        // 如果没有建立通道，跳过开启LED
        if (ads1 == 0 && ads2 == 0)
        {
          continue;
        }

        // 开启LED
        TLC59116_2_SetPwm(ch, g_led_brightness[8 + led]);

        // 延时5us
        delay_us(25);

        // led与ads1建立了通道
        if (ads1 != 0)
        {
          // 选择第一个ADS1299芯片
          ADS1299_1_CS_Low();

          ADS1299_Start();

          // 等待PB0为低电平
          while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) != GPIO_PIN_RESET)
          {
          }

          // 读取ADS12991数据寄存器
          HAL_SPI_TransmitReceive(&hspi1, spi_tx_buffer, spi_ads_1_rx_buffer, 27, 100U);

          // 关闭采集
          ADS1299_Stop();

          ADS1299_1_CS_High();

          for (uint8_t i = 0; i < 8; i++)
          {
            uint8_t set = is_bit_set(ads1, i);
            if (1 == set)
            {
              int32_t value =  ads1299_24_to_32(&spi_ads_1_rx_buffer[3 + i * 3]);
              g_ads_data[g_ads_data_count++] = value - ads_1_origin[i];
            }
          }
        }

        // led与ads2建立了通道
        if (ads2 != 0)
        {
          // 选择第一个ADS1299芯片
          ADS1299_2_CS_Low();

          ADS1299_Start();

          // 等待PA0为低电平
          while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) != GPIO_PIN_RESET)
          {
          }

          // 读取ADS12992数据寄存器
          HAL_SPI_TransmitReceive(&hspi1, spi_tx_buffer, spi_ads_2_rx_buffer, 27, 100U);

          // 关闭采集
          ADS1299_Stop();

          ADS1299_2_CS_High();

          for (uint8_t i = 0; i < 8; i++)
          {
            uint8_t set = is_bit_set(ads2, i);
            if (1 == set)
            {
              int32_t value =  ads1299_24_to_32(&spi_ads_2_rx_buffer[3 + i * 3]);
              g_ads_data[g_ads_data_count++] = value - ads_2_origin[i];
            }
          }
        }

        // 关闭LED
        TLC59116_2_SetPwm(ch, 0);
      }

      g_sample_idx++;
    }

    // 已经采了3次数据了，该发送了
    if (g_sample_idx >= 3)
    {
      g_sample_idx = 0;

      // 数据头和型号
      g_packet[0] = 0x84U;
      g_packet[1] = 0x6FU;
      g_packet[2] = 0x0BU;

      // 数据个数
      memcpy(g_packet + 3, &g_ads_data_count, sizeof(g_ads_data_count));

      // 数据包序号
      memcpy(g_packet + 5, &g_packet_num, sizeof(g_packet_num));

      g_packet[9] = 0x00U;
      g_packet[10] = 0x00U;
      g_packet[11] = 0x00U;
      g_packet[12] = 0x00U;

      // 复制数据
      memcpy(g_packet + 13, &g_ads_data, 4 * g_ads_data_count);

      // TODO: 处理数据陀螺仪
      memset(g_packet + 13 + 4 * g_ads_data_count, 0, 18);

      g_packet[13 + 4 * g_ads_data_count + 18] = get_battery_level();

      g_packet[13 + 4 * g_ads_data_count + 19] = 0;

      // 发送数据
      HAL_UART_Transmit(&huart3, g_packet, 13 + g_ads_data_count * 4 + 20, HAL_MAX_DELAY);


      // 发送完毕，重置数据个数，从头开始写数据
      g_ads_data_count = 0;

      // 每发送一次数据，序号加1
      g_packet_num++;
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
}

void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);

    while ((int32_t)(DWT->CYCCNT - start) < (int32_t)ticks);
}

uint8_t is_bit_set(uint8_t value, uint8_t bit)
{
    if (bit > 7) return 0;  // 8位范围检查
    return (value & (1 << bit)) != 0;
}

/**
  * @brief   配置定时器6的采样率
  * @param   rate  采样率，单位为Hz
  * @retval  None
  */
void configure_tim6_for_sample_rate(uint32_t rate, uint16_t* new_psc, uint16_t* new_arr)
{
  // 定时器时钟为64MHz
  // 定时器频率 = 定时器时钟 / ((PSC + 1) * (ARR + 1))

  if (rate == 0) rate = 10;  // 默认10Hz

  if (rate <= 100)
  {
    // 低采样率：使用较大的预分频
    *new_psc = 6399;   // Prescaler = 6399，定时器时钟 = 64MHz / 6400 = 10kHz
    *new_arr = (10000 / rate) - 1;
  }
  else if (rate <= 1000)
  {
    // 中采样率
    *new_psc = 639;    // Prescaler = 639，定时器时钟 = 64MHz / 640 = 100kHz
    *new_arr = (100000 / rate) - 1;
  }
  else
  {
    // 高采样率
    *new_psc = 63;     // Prescaler = 63，定时器时钟 = 64MHz / 64 = 1MHz
    *new_arr = (1000000 / rate) - 1;
  }
}

/**
  * @brief   处理GPIO外部中断，连接ADS1299芯片的DRDY引脚，当DRDY引脚拉低时，触发中断，读取数据寄存器的内容
  * @param   GPIO_Pin  GPIO引脚号
  * @retval  None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  // PB0(连接第一个ADS1299的DRDY引脚) 下降沿中断处理逻辑
  if (GPIO_Pin == GPIO_PIN_0)
  {
    // 将ADS1299_1的CS引脚拉低，读取数据寄存器
    ADS1299_1_CS_Low();

    // 读取ADS1299_1的数据寄存器
    HAL_SPI_TransmitReceive_DMA(&hspi1,
                                spi_tx_buffer,
                                spi_ads_1_rx_buffer,
                                27);
  }
}

/**
  * @brief   SPI DMA收发完成回调函数
  * @param   hspi  SPI句柄
  * @retval  None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI1)
  {
    ADS1299_1_CS_High();
  }
}

/**
  * @brief   处理USART3的接收中断，USART3用于与WIFI模块通信
  * @param   huart  UART句柄
  * @retval  None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  // 处理USART3的接收中断，USART3用于与WIFI模块通信
  if (huart->Instance == USART3)
  {
    // 将数据写入环形缓冲区
    rb_write(g_usart3_rx_byte);
    HAL_UART_Receive_IT(&huart3, &g_usart3_rx_byte, 1U);
  }
}

/**
  * @brief   TIM6周期溢出回调函数
  * @param   htim  定时器句柄
  * @retval  None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    /* 设置定时器6中断标志位 */
    g_tim6_ready = 1;
  }
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
