/**
 ******************************************************************************
 * @file    drv_tim.h
 * @brief   定时器驱动
 ******************************************************************************
 * @attention
 * 
 * Copyright 2024 RobotPilots
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_TIM_H
#define __DRV_TIM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
//void Telescope_Up(void);
//void Telescope_Down(void);
//void Telescope_Sleep(void);
void TIM1_Init(void);
void TIM4_Init(void);


/* Servo functions */
#endif
