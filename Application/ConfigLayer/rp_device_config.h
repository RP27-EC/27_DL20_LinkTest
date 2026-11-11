#ifndef __RP_DEVICE_CONFIG_H
#define __RP_DEVICE_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "rp_driver_config.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* 设备层 --------------------------------------------------------------------*/
/**
 *	@brief	设备id列表
 *	@class	device
 */
typedef enum {
	DEV_ID_IMU = 0,
  DEV_ID_IMU_EX,
	DEV_ID_RC,
	DEV_ID_VISION,
	DEV_ID_CNT,
} dev_id_t;

/**
 *	@brief	设备工作状态(通用)
 *	@class	device
 */
typedef enum {
	DEV_ONLINE,
	DEV_OFFLINE,
} dev_work_state_t;


/**
 * @brief 未初始化：DEV_RESET_NO 初始化完成:DEV_RESET_OK
 * 
 */
typedef enum DEV_RESET_STATE
{
	DEV_RESET_NO,
	DEV_RESET_OK,
}Dev_Reset_State_e;

/**
 *	@brief	错误代码(通用)
 *  @note   可自定义设备错误代码类型并替换变量errno的变量类型，如
 *          typedef enum {
 *              IMU_NONE_ERR,
 *              IMU_ID_ERR,
 *              IMU_COM_FAILED,
 *              IMU_DEV_NOT_FOUND,
 *              ...
 *          } imu_errno_t;
 *          
 *          typedef struct imu_sensor_struct {
 *              ...
 *	            imu_errno_t errno;
 *              ...	
 *          } imu_sensor_t;
 *	@class	device
 */
typedef enum {
	NONE_ERR,		// 正常(无错误)
	DEV_ID_ERR,		// 设备ID错误
	DEV_INIT_ERR,	// 设备初始化错误
	DEV_DATA_ERR,	// 设备数据错误
} dev_errno_t;

typedef enum {
	DAIL,		  //	CAN1     0x207	
	IMAGE,        //	CAN1	 0x206
	TELESCOPE,    //	CAN1	 0x205
	
	FRIC_B_UP,	//		CAN2	 0x201
	FRIC_F_UP, 	//		CAN2	 0x202
	FRIC_B_R, 	//  	CAN2	 0x203
	FRIC_B_L,	//		CAN2	 0x204
	FRIC_F_R, 	//		CAN2	 0x205
	GIMB_P, 	//		CAN2	 0x206
	FRIC_F_L,   //		CAN2	 0x207
	
	
	RM_MOTOR_LIST,
} dev_rm_motor_list_e;			  //  Yaw轴kt电机CAN1  0x142 2个包  
									  //CAN1 底盘1个包
									  //UI四个，整车信息1个（5ms发一次，也就是0.2个包）
									  //收game_robot_status 0.01个包（10Hz），
									  //power_heat_data0.05个包
								      //shoot_data 子弹发射后下主控才发送，忽略不计
								      //总共2+2+1+0.2+0.05+0



#endif
