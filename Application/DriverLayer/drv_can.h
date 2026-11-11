/**
  ******************************************************************************
  * @file    drv_can.h
  * @brief   CAN driver header file
  ******************************************************************************
  * @attention
  * 
  * Copyright 2024 RobotPilots
  ******************************************************************************
  */
#ifndef __DRV_CAN_H
#define __DRV_CAN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "rp_driver_config.h"

/* Exported variables --------------------------------------------------------*/
extern uint8_t CAN1_200_DATA[8];
extern uint8_t CAN1_1FF_DATA[8];
extern uint8_t CAN2_200_DATA[8];
extern uint8_t CAN2_1FF_DATA[8];
extern uint8_t CAN1_2FF_DATA[8];
extern uint8_t CAN2_2FF_DATA[8];

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief CAN1\CAN2
  */
typedef struct {
	CAN_RxHeaderTypeDef header;
	uint8_t				data[8];
} CAN_RxFrameTypeDef;



extern CAN_TxHeaderTypeDef CAN_TxHeadeType;
/* Exported functions --------------------------------------------------------*/
void CAN_Filter_Init(void);
void CAN1_CMD_200(void);
void CAN1_CMD_1FF(void);
void CAN2_CMD_200(void);
void CAN2_CMD_1FF(void);
void int16_to_uint8(uint8_t *data, int16_t *dat);

void HAL_CAN_TxHeadeInit(uint16_t ID);
void CAN1_SendData(uint32_t StdId, uint8_t *data);
void CAN2_SendData(uint32_t StdId, uint8_t *data);
HAL_StatusTypeDef CAN_SendData(CAN_HandleTypeDef *hcan, uint32_t stdId, uint8_t *dat);

#endif
