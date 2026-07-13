/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    RingBuffer.h
  * @brief   This file contains all the function prototypes for
  *          the RingBuffer.c file
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
#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* 环形缓冲区大小，必须为2的幂 */
#define RB_SIZE 256U
#define RB_MASK (RB_SIZE - 1U)

/**
 * @brief 环形缓冲区结构体
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
typedef struct
{
  uint8_t buf[RB_SIZE];         /**< 缓冲区 */
  volatile uint32_t head;       /**< 头指针(写入位置) */
  volatile uint32_t tail;       /**< 尾指针(读取位置) */
} RingBuffer;

/**
 * @brief 初始化环形缓冲区
 * @param[in] rb 环形缓冲区指针
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
void RB_Init(RingBuffer *rb);

/**
 * @brief 批量写入数据到环形缓冲区
 * @param[in] rb 环形缓冲区指针
 * @param[in] data 要写入的数据指针
 * @param[in] len 要写入的数据长度
 * @return 实际写入的字节数
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
uint32_t RB_Write(RingBuffer *rb, const uint8_t *data, uint32_t len);

/**
 * @brief 批量从环形缓冲区读取数据
 * @param[in] rb 环形缓冲区指针
 * @param[out] data 读取到的数据指针
 * @param[in] len 最大读取长度
 * @return 实际读取的字节数
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
uint32_t RB_Read(RingBuffer *rb, uint8_t *data, uint32_t len);

/**
 * @brief 写入单个字节到环形缓冲区
 * @param[in] rb 环形缓冲区指针
 * @param[in] data 要写入的字节
 * @return true=写入成功, false=缓冲区已满
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
bool RB_WriteByte(RingBuffer *rb, uint8_t data);

/**
 * @brief 从环形缓冲区读取单个字节
 * @param[in] rb 环形缓冲区指针
 * @param[out] data 读取到的字节指针
 * @return true=读取成功, false=缓冲区为空
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
bool RB_ReadByte(RingBuffer *rb, uint8_t *data);

/**
 * @brief 获取环形缓冲区中可用的字节数
 * @param[in] rb 环形缓冲区指针
 * @return 可用字节数
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
uint32_t RB_Available(const RingBuffer *rb);

/**
 * @brief 判断环形缓冲区是否为空
 * @param[in] rb 环形缓冲区指针
 * @return true=空, false=非空
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
bool RB_IsEmpty(const RingBuffer *rb);

/**
 * @brief 判断环形缓冲区是否已满
 * @param[in] rb 环形缓冲区指针
 * @return true=满, false=未满
 *
 * @author  黄佳兴
 * @version 0.1
 * @date    2026-07-10
 */
bool RB_IsFull(const RingBuffer *rb);

#ifdef __cplusplus
}
#endif
#endif /*__RINGBUFFER_H__ */

