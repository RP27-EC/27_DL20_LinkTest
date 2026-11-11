/**
 ******************************************************************************
 * @file    can_protocol.h
 * @brief   CAN通信协议层
 ******************************************************************************
 * @attention
 *
 * Copyright 2024 RobotPilots
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_PROTOCOL_H
#define __CAN_PROTOCOL_H

/* Includes ------------------------------------------------------------------*/
#include "driver.h"
#include "device.h"

/* Exported macro ------------------------------------------------------------*/
/* 下主控CAN ID */
#define SLAVE_TX_ID 
#define SLAVE_RX_ID 

/*
//CAN1
#define ID_GIMB_YAW 	0x142
#define ID_DAIL 		0x207 //0x1FF  45
#define ID_IMAGE		0x206 //0x1FF  23
#define ID_TELESCOPE	0x205 //0x1FF  01

//CAN2
#define ID_FRIC_B_UP 	0x201 //0x200  01
#define ID_FRIC_F_UP 	0x202 //0x200  23
#define ID_FRIC_B_R 	0x203 //0x200  45
#define ID_FRIC_B_L 	0x204 //0x200  67
#define ID_FRIC_F_R 	0x205 //0x1FF  01
#define ID_GIMB_P 		0x206 //0x1FF  23
#define ID_FRIC_F_L 	0x207 //0x1FF  45
*/


/* Exported functions --------------------------------------------------------*/
void CAN1_rxDataHandler(uint32_t canId, uint8_t *rxBuf);
void CAN2_rxDataHandler(uint32_t canId, uint8_t *rxBuf);
void CAN_SendAll(void);
void CAN_Send(void);
void CAN_SendAllZero(void);


#endif
