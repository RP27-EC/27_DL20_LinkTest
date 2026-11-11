#include "can_protocol.h"

/**
 *  @brief  CAN1 接收数据
 */
void CAN1_rxDataHandler(uint32_t rxId, uint8_t *rxBuf)
{
	switch (rxId)
	{
		case 0x0b:
		{
			L_Wheel.rx(&L_Wheel, rxBuf);
			
			break;
		}
		case 0x011://接收ID
		{
			Yaw_Motor.rx(&Yaw_Motor, rxBuf);
			break;
		}
		case 0x205:
		{
			R_Fric.rx(&R_Fric, rxBuf);
			break;
		}

		default:
			break;
	}
}
/**
 *  @brief  CAN2 接收数据
 */
void CAN2_rxDataHandler(uint32_t canId, uint8_t *rxBuf)
{
	
	switch (canId)
	{
		
		default:
			break;
	}
}
