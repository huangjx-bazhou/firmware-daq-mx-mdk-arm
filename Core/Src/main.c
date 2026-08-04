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
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "rtc.h"
#include "sdmmc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include <stdio.h>
#include "ADS1299.h"
#include "BatteryLevel.h"
#include "E101-C5WN8-PS.h"
#include "GY-95T.h"
#include "RingBuffer.h"
#include "SD.h"
#include "TLC59116.h"
#include "Utils.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define CALIBRATE_FREQUENCY 15000U // 校准频率每15000次校准一次

#define PACKET_SIZE 2048U // 数据包大小, 包括2字节头

// 如果组包速度大于DMA发送速度
#define PACKET_COUNT 8U // 数据包数量

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// TIM6中断标志
static volatile bool g_tim6_ready = false;

// USART3发送忙标志
static volatile bool g_usart3_tx_busy = false;


// ADS1299数据
static int32_t g_ads_data[LED_COUNT * RECVIVER_COUNT * CHANNEL_COUNT_PER_LED];

// 数据个数
static uint16_t g_ads_data_count = 0U;

// 采集索引
static uint8_t g_sample_idx = 0U;

// 数据包序号
static uint32_t g_packet_num = 0U;

// 数据包,需要发送到上位机
static uint8_t g_packet[PACKET_COUNT][PACKET_SIZE];

// 新数据包索引
static int8_t g_new_packet_index = -1;

// 正在发送数据包索引
static int8_t g_tx_packet_index = -1;

// SD卡正在写入的数据包索引
static int8_t g_sd_packet_index = -1;

static uint16_t g_tx_ads_data_count = 0U;

static volatile uint8_t g_tx_head = 0U;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/**
  * @brief  根据采样率配置定时器6
  * @param rate 采样率（Hz）
  * @param new_psc 新的预分频器值（0-65535）
  * @param new_arr 新的自动重载值（0-65535）
  */
static void configure_tim6_for_sample_rate(uint32_t rate, uint16_t* new_psc, uint16_t* new_arr);

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

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C2_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  MX_TIM6_Init();
  MX_ADC3_Init();
  MX_TIM7_Init();
  MX_USART2_UART_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  MX_RTC_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  // 初始化数据包，提前设置头字节为846F0B
  for (uint8_t i = 0; i < PACKET_COUNT; ++i)
  {
    g_packet[i][0] = 0x84;
    g_packet[i][1] = 0x6F;
    g_packet[i][2] = 0x0B;
    g_packet[i][3] = 0x00;
  }

  SD_Mount();
	
	  // 清理HAL_SD_Init阶段可能产生的UART2错误标志和残留数据
  __HAL_UART_CLEAR_OREFLAG(&huart2);
  __HAL_UART_CLEAR_NEFLAG(&huart2);
  __HAL_UART_CLEAR_FEFLAG(&huart2);
  __HAL_UART_CLEAR_PEFLAG(&huart2);
  __HAL_UART_SEND_REQ(&huart2, UART_RXDATA_FLUSH_REQUEST);

  // 清理HAL_SD_Init阶段可能产生的UART3错误标志和残留数据
  __HAL_UART_CLEAR_OREFLAG(&huart3);
  __HAL_UART_CLEAR_NEFLAG(&huart3);
  __HAL_UART_CLEAR_FEFLAG(&huart3);
  __HAL_UART_CLEAR_PEFLAG(&huart3);
  __HAL_UART_SEND_REQ(&huart3, UART_RXDATA_FLUSH_REQUEST);

  // 初始化USART2接收中断
  HAL_UART_Receive_IT(&huart2, &g_gy95t_usart2_rx_byte, 1U);

  // 初始化USART3接收中断
  HAL_UART_Receive_IT(&huart3, &g_wifi_usart3_rx_byte, 1U);

  HAL_TIM_Base_Start_IT(&htim6);

  HAL_TIM_Base_Start_IT(&htim7);

  DWT_Init();

  /* 初始化GY95T */
  GY95T_Init();

  /* 初始化WIFI */
  WIFI_Init();

  /* 初始化所有(2个)TLC59116为PWM模式，设置所有通道的PWM值为0 */
  TLC59116_Init();

  /* 初始化第1个ADS1299芯片 */
  ADS1299_1_Init();

  /* 初始化第2个ADS1299芯片 */
  ADS1299_2_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* SD卡检测去抖：中断只登记事件，在主循环中30ms后判稳 */
    if (g_sd_card_detect_irq_pending)
    {
      if ((HAL_GetTick() - g_sd_card_detect_irq_tick) >= SD_CARD_DEBOUNCE_MS)
      {
        g_sd_card_detect_irq_pending = false;

        bool old_detected = g_sd_card_detected;
        SD_Detect();

        if (old_detected != g_sd_card_detected)
        {
          if (g_sd_card_detected)
          {
            // 检测到卡插入了
            MX_SDMMC1_SD_Init();

            SD_Mount();
          }
          else
          {
            // 检测到卡拔出了
            g_sd_card_initialized = false;
            g_sd_card_mounted = false;
            g_file_opened = false;
          }

          SD_SendInfo();
        }
      }
    }

    {
      static uint32_t power_key_start_time = 0U;
      static bool power_key_pressed = false;

      if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5) == GPIO_PIN_SET)
      {
        if (!power_key_pressed)
        {
          power_key_start_time = HAL_GetTick();
          power_key_pressed = true;
        }
        else
        {
          if ((HAL_GetTick() - power_key_start_time) >= 1500U)
          {
            SD_CloseFile();
            HAL_GPIO_WritePin(Power_GPIO_Port, Power_Pin, GPIO_PIN_RESET);
            while (1) {}
          }
        }
      }
      else
      {
        power_key_pressed = false;
      }
    }

    /* 1. 处理WIFI串口数据 */
    if (RB_Available(&g_wifi_rb) > 0)
    {
      uint8_t byte;
      RB_ReadByte(&g_wifi_rb, &byte);
      WIFI_AssembleCommand(byte);
    }

    /* 2. 处理是否发送SD卡信息标志 */
    if (g_send_sd_info_flag)
    {
      g_send_sd_info_flag = false;

      SD_SendInfo();
    }

    /* 2. 处理GY95T串口数据 */
    if (RB_Available(&g_gy95t_rb) > 0)
    {
      uint8_t byte;
      RB_ReadByte(&g_gy95t_rb, &byte);
      GY95T_ProcessByte(byte);
    }

    /* 3. 处理离线存储标志改变标志 */
    if (g_storage_flag_changed)
    {
      g_storage_flag_changed = false;
    }

    /* 4. 处理采样率改变标志 */
    if (g_sample_rate_changed)
    {
      g_sample_rate_changed = false;

      /* 新的预分频器值 */
      uint16_t new_psc = 0U;
      /* 新的自动重载值 */
      uint16_t new_arr = 0U;

      /* 配置定时器6的采样率 */
      configure_tim6_for_sample_rate(g_sample_rate, &new_psc, &new_arr);

      /* 采样率改变，更新定时器6的周期 */
      __HAL_TIM_SET_PRESCALER(&htim6, new_psc);
      __HAL_TIM_SET_AUTORELOAD(&htim6, new_arr);
      HAL_TIM_GenerateEvent(&htim6, TIM_EVENTSOURCE_UPDATE);
    }

    /* 5. 处理开始采样标志改变标志 */
    if (g_start_flag_changed)
    {
      g_start_flag_changed = false;

      /* 采样 */
      if (g_start_flag)
      {
        RTC_TimeTypeDef rtc_time = {0};
        RTC_DateTypeDef rtc_date = {0};
        char file_name[24] = {0};

        /* 读取RTC时间并生成文件名: yyyyMMdd-hhmmss */
        HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
        (void)snprintf(file_name, sizeof(file_name), "0:%04u%02u%02u-%02u%02u%02u",
                       (unsigned int)(2000U + rtc_date.Year),
                       (unsigned int)rtc_date.Month,
                       (unsigned int)rtc_date.Date,
                       (unsigned int)rtc_time.Hours,
                       (unsigned int)rtc_time.Minutes,
                       (unsigned int)rtc_time.Seconds);
        SD_SetFileName(file_name);

        SD_OpenFile();

        /* 开始GY95T的连续输出 */
        GY95T_Start();



        
      }
      else
      {
        SD_CloseFile();

        /* 当停止采样时，停止GY95T的连续输出 */
        GY95T_Stop();

        g_packet_num = 0U;
        g_timestamp = 0U;
      }
    }

    /* 6. 处理USART3发送忙标志，USART3发送处于空闲状态,并且有新的数据包需要发送 */
    if (!g_usart3_tx_busy && (g_tx_packet_index != g_new_packet_index))
    {
      /* 设置为发送忙状态 */
      g_usart3_tx_busy = true;

      /* 下一个要发送的数据包索引 */
      g_tx_packet_index = (g_tx_packet_index + 1) % PACKET_COUNT;


      //SD_OpenFile();
      UINT bw = 0U;
      FRESULT res = SD_WriteFile(g_packet[g_tx_packet_index], 14 + g_tx_ads_data_count * 4 + 20, &bw);
      //SD_CloseFile();

      HAL_UART_Transmit_DMA(&huart3, g_packet[g_tx_packet_index], 14 + g_tx_ads_data_count * 4 + 20);
    }

    /* 7. 处理定时器6中断 */
    if (g_tim6_ready && g_start_flag)
    {
      g_tim6_ready = false;


      uint32_t tim6_start = DWT->CYCCNT;

      /* 循环开启TLC59116-1的16个通道 */
      for (uint8_t ch = 0; ch < LED_COUNT; ch++)
      {
        /* 第几个LED */
        uint8_t led = ch / 2;

        uint8_t ads1 =  g_channel_mask[led * 2];

        uint8_t ads2 =  g_channel_mask[led * 2 + 1];

        // 如果没有建立通道，跳过开启LED
        if (ads1 == 0 && ads2 == 0)
        {
          continue;
        }

        uint32_t turn_on_led_start = DWT->CYCCNT;

        // 开启LED
        TLC59116_1_SetPwm(ch, g_led_brightness[led]);

        // TODO: 待确认延时10us
        Delay_US(20);

        uint32_t ads1_channel_start = DWT->CYCCNT;

        if (ads1 != 0)
        {
          ADS1299_1_CS_Low();
          ADS1299_Start_ByCmd();
          ADS1299_1_CS_High();
        }

        if (ads2 != 0)
        {
          ADS1299_2_CS_Low();
          ADS1299_Start_ByCmd();
          ADS1299_2_CS_High();
        }

        // led与ads1建立了通道
        if (ads1 != 0)
        {
          uint32_t ads1299_1_wait_drdy_start = DWT->CYCCNT;

          // 等待DRDY引脚为低电平
          while (HAL_GPIO_ReadPin(ADS1299_1_DRDY_GPIO_Port, ADS1299_1_DRDY_Pin) != GPIO_PIN_RESET)
          {
          }

          uint32_t ads1299_1_read_data_start = DWT->CYCCNT;

          ADS1299_1_CS_Low();

          // 读取ADS12991数据寄存器
          HAL_SPI_TransmitReceive(&hspi1, ads_tx_buffer, ads_1_rx_buffer, 27, 100U);

          uint32_t ads1299_1_stop_start = DWT->CYCCNT;

          uint32_t ads1299_1_cs_high_start = DWT->CYCCNT;

          ADS1299_1_CS_High();

          uint32_t ads1299_1_process_data_start = DWT->CYCCNT;

          for (uint8_t i = 0; i < 8; i++)
          {
            uint8_t set = Is_Bit_Set(ads1, i);
            if (1 == set)
            {
              int32_t value =  Sign_Extend_24_to_32(&ads_1_rx_buffer[3 + i * 3]);
              g_ads_data[g_ads_data_count++] = value - ads_1_origin[i];
            }
          }

          uint32_t ads1299_1_process_data_end = DWT->CYCCNT;
        }

        uint32_t ads2_channel_start = DWT->CYCCNT;

        // led与ads2建立了通道
        if (ads2 != 0)
        {

          uint32_t ads1299_2_wait_drdy_start = DWT->CYCCNT;

          // 等待DRDY引脚为低电平
          while (HAL_GPIO_ReadPin(ADS1299_2_DRDY_GPIO_Port, ADS1299_2_DRDY_Pin) != GPIO_PIN_RESET)
          {
          }

          uint32_t ads1299_2_read_data_start = DWT->CYCCNT;

          ADS1299_2_CS_Low();

          // 读取ADS12992数据寄存器
          HAL_SPI_TransmitReceive(&hspi1, ads_tx_buffer, ads_2_rx_buffer, 27, 100U);

          uint32_t ads1299_2_stop_start = DWT->CYCCNT;

          uint32_t ads1299_2_cs_high_start = DWT->CYCCNT;

          ADS1299_2_CS_High();

          uint32_t ads1299_2_process_data_start = DWT->CYCCNT;

          for (uint8_t i = 0; i < 8; i++)
          {
            uint8_t set = Is_Bit_Set(ads2, i);
            if (1 == set)
            {
              int32_t value =  Sign_Extend_24_to_32(&ads_2_rx_buffer[3 + i * 3]);
              g_ads_data[g_ads_data_count++] = value - ads_2_origin[i];
            }
          }

          uint32_t ads1299_2_process_data_end = DWT->CYCCNT;
        }

        if (ads1 != 0)
        {
          ADS1299_1_CS_Low();
          ADS1299_Stop_ByCmd();
          ADS1299_1_CS_High();
        }

        if (ads2 != 0)
        {
          ADS1299_2_CS_Low();
          ADS1299_Stop_ByCmd();
          ADS1299_2_CS_High();
        }

        uint32_t turn_off_led_start = DWT->CYCCNT;

        // 关闭LED
        TLC59116_1_SetPwm(ch, 0);

        uint32_t turn_off_led_end = DWT->CYCCNT;
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

        uint32_t turn_on_led_start = DWT->CYCCNT;

        // 开启LED
        TLC59116_2_SetPwm(ch, g_led_brightness[8 + led]);

        // TODO: 待确认延时10us
        Delay_US(20);

        uint32_t ads1_channel_start = DWT->CYCCNT;

        if (ads1 != 0)
        {
          ADS1299_1_CS_Low();
          ADS1299_Start_ByCmd();
          ADS1299_1_CS_High();
        }

        if (ads2 != 0)
        {
          ADS1299_2_CS_Low();
          ADS1299_Start_ByCmd();
          ADS1299_2_CS_High();
        }

        // led与ads1建立了通道
        if (ads1 != 0)
        {


          uint32_t ads1299_1_wait_drdy_start = DWT->CYCCNT;

          // 等待DRDY引脚为低电平
          while (HAL_GPIO_ReadPin(ADS1299_1_DRDY_GPIO_Port, ADS1299_1_DRDY_Pin) != GPIO_PIN_RESET)
          {
          }

          ADS1299_1_CS_Low();

          uint32_t ads1299_1_read_data_start = DWT->CYCCNT;

          // 读取ADS12991数据寄存器
          HAL_SPI_TransmitReceive(&hspi1, ads_tx_buffer, ads_1_rx_buffer, 27, 100U);

          uint32_t ads1299_1_stop_start = DWT->CYCCNT;

          uint32_t ads1299_1_cs_high_start = DWT->CYCCNT;

          ADS1299_1_CS_High();

          uint32_t ads1299_1_process_data_start = DWT->CYCCNT;

          for (uint8_t i = 0; i < 8; i++)
          {
            uint8_t set = Is_Bit_Set(ads1, i);
            if (1 == set)
            {
              int32_t value =  Sign_Extend_24_to_32(&ads_1_rx_buffer[3 + i * 3]);
              g_ads_data[g_ads_data_count++] = value - ads_1_origin[i];
            }
          }

          uint32_t ads1299_1_process_data_end = DWT->CYCCNT;
        }

        uint32_t ads2_channel_start = DWT->CYCCNT;

        // led与ads2建立了通道
        if (ads2 != 0)
        {
          uint32_t ads1299_2_wait_drdy_start = DWT->CYCCNT;

          // 等待DRDY引脚为低电平
          while (HAL_GPIO_ReadPin(ADS1299_2_DRDY_GPIO_Port, ADS1299_2_DRDY_Pin) != GPIO_PIN_RESET)
          {
          }

          uint32_t ads1299_2_read_data_start = DWT->CYCCNT;

          ADS1299_2_CS_Low();

          // 读取ADS12992数据寄存器
          HAL_SPI_TransmitReceive(&hspi1, ads_tx_buffer, ads_2_rx_buffer, 27, 100U);

          uint32_t ads1299_2_stop_start = DWT->CYCCNT;

          uint32_t ads1299_2_cs_high_start = DWT->CYCCNT;

          ADS1299_2_CS_High();

          uint32_t ads1299_2_process_data_start = DWT->CYCCNT;

          for (uint8_t i = 0; i < 8; i++)
          {
            uint8_t set = Is_Bit_Set(ads2, i);
            if (1 == set)
            {
              int32_t value =  Sign_Extend_24_to_32(&ads_2_rx_buffer[3 + i * 3]);
              g_ads_data[g_ads_data_count++] = value - ads_2_origin[i];
            }
          }

          uint32_t ads1299_2_process_data_end = DWT->CYCCNT;
        }


        if (ads1 != 0)
        {
          ADS1299_1_CS_Low();
          ADS1299_Stop_ByCmd();
          ADS1299_1_CS_High();
        }

        if (ads2 != 0)
        {
          ADS1299_2_CS_Low();
          ADS1299_Stop_ByCmd();
          ADS1299_2_CS_High();
        }

        uint32_t turn_off_led_start = DWT->CYCCNT;


        // 关闭LED
        TLC59116_2_SetPwm(ch, 0);

        uint32_t turn_off_led_end = DWT->CYCCNT;

      }

      g_sample_idx++;
    }

    // 采样次数大于指定次数就发送
    if (g_sample_idx >= 1)
    {
      g_sample_idx = 0;

      // 新数据包索引
      int8_t new_packet_index = (g_new_packet_index + 1) % PACKET_COUNT;;
      if (new_packet_index == g_tx_packet_index)
      {
      }
      else
      {
        g_new_packet_index = new_packet_index;
      }

      // 数据个数
      memcpy(g_packet[g_new_packet_index] + 4, &g_ads_data_count, sizeof(g_ads_data_count));

      // 数据包序号
      memcpy(g_packet[g_new_packet_index] + 6, &g_packet_num, sizeof(g_packet_num));

      // 时间戳
      memcpy(g_packet[g_new_packet_index] + 10, (const void *)&g_timestamp, sizeof(g_timestamp));

      // 复制数据
      memcpy(g_packet[g_new_packet_index] + 14, &g_ads_data, 4 * g_ads_data_count);

      /* GY95T数据 */
      memcpy(g_packet[g_new_packet_index] + 14 + 4 * g_ads_data_count, g_gy95t_data, GY95T_DATA_SIZE);

      g_packet[g_new_packet_index][14 + 4 * g_ads_data_count + GY95T_DATA_SIZE] = Get_Battery_Level();

      g_packet[g_new_packet_index][14 + 4 * g_ads_data_count + GY95T_DATA_SIZE + 1] = 0;

      g_tx_ads_data_count = g_ads_data_count;
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = 8;
  RCC_OscInitStruct.PLL.PLLQ = 20;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
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
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_SDMMC
                              |RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.PLL2.PLL2M = 2;
  PeriphClkInitStruct.PLL2.PLL2N = 32;
  PeriphClkInitStruct.PLL2.PLL2P = 20;
  PeriphClkInitStruct.PLL2.PLL2Q = 10;
  PeriphClkInitStruct.PLL2.PLL2R = 10;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL2;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_PLL2;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief   配置定时器6的采样率
  * @param   rate  采样率，单位为Hz
  * @retval  None
  */
void configure_tim6_for_sample_rate(uint32_t rate, uint16_t* new_psc, uint16_t* new_arr)
{
  // 定时器时钟为100MHz
  // 定时器频率 = 定时器时钟 / ((PSC + 1) * (ARR + 1))

  if (rate == 0) rate = 50;  // 默认10Hz

  if (rate <= 100)
  {
    // 低采样率：使用较大的预分频
    *new_psc = 9999;   // Prescaler = 9999，定时器时钟 = 100MHz / 10000 = 10kHz
    *new_arr = (10000 / rate) - 1;
  }
  else if (rate <= 1000)
  {
    // 中采样率
    *new_psc = 999;    // Prescaler = 999，定时器时钟 = 100MHz / 1000 = 100kHz
    *new_arr = (100000 / rate) - 1;
  }
  else
  {
    // 高采样率
    *new_psc = 99;     // Prescaler = 99，定时器时钟 = 100MHz / 100 = 1MHz
    *new_arr = (1000000 / rate) - 1;
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (Card_Detect_Pin == GPIO_Pin)
  {
    // 双边沿触发下仅记录事件，实际状态在主循环中去抖判稳
    g_sd_card_detect_irq_tick = HAL_GetTick();
    g_sd_card_detect_irq_pending = true;
  }
}

/**
 * @brief   USART的接收完成回调
 * @param   huart  UART句柄
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (USART2 == huart->Instance)
  {
    RB_WriteByte(&g_gy95t_rb, g_gy95t_usart2_rx_byte);
    HAL_UART_Receive_IT(&huart2, &g_gy95t_usart2_rx_byte, 1U);
  }
  else if (USART3 == huart->Instance)
  {
    RB_WriteByte(&g_wifi_rb, g_wifi_usart3_rx_byte);
    HAL_UART_Receive_IT(&huart3, &g_wifi_usart3_rx_byte, 1U);
  }
}

/**
 * @brief   USART的发送完成回调
 * @param   huart  UART句柄
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (USART3 == huart->Instance)
  {
    g_usart3_tx_busy = false;
  }
}

/**
 * @brief   定时器周期溢出回调函数
 * @param   htim  定时器句柄
 * @retval  None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (TIM6 == htim->Instance)
  {
    g_tim6_ready = true;
  }
  else if (TIM7 == htim->Instance)
  {
    g_timestamp++;
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
  if (true == g_sd_card_initializing)
  {
    g_sd_card_initialized = false;
    return;
  }

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
