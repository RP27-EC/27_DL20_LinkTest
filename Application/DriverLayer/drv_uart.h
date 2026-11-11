/**
 ******************************************************************************
 * @file        drv_uart.h
 * @author      RobotPilots@2020
 * @brief       UART Driver Package(Based on HAL).
 ******************************************************************************
 * @attention
 * 
 * Copyright 2020 RobotPilots
 * 
 * @Version     V1.0
 * @date        15-August-2020
 ******************************************************************************
 */
#ifndef __DRV_UART_H
#define __DRV_UART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "rc_sensor.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void DRV_UART_IRQHandler(UART_HandleTypeDef *huart);
void USART1_Init(void);
void USART2_Init(void);
void USART3_Init(void);
void USART5_Init(void);
void USART6_Init(void);
void  UART_printf(char *format, ...);
#define USART1_RX_BUF_LEN    128
extern uint8_t usart1_dma_rxbuf[USART1_RX_BUF_LEN];
extern uint8_t rc_offline_cnt;
extern DMA_HandleTypeDef hdma_usart1_rx;
#endif
