#ifndef __DM_MOTOR_H
#define __DM_MOTOR_H

#include "rp_config.h"
#include "arm_math.h"
#include "HT_Motor.h"
#include "drv_can.h"
#include "drv_tick.h"
#include "motor_def.h"
#include "rp_math.h"
#ifndef __HT_MOTOR_H
/*电机指令集*/
typedef enum Motor_MIT_Command_enum_e
{
	Enter_Motor_Mode,//使能电机控制(指示灯变绿)
	Exit_Motor_Mode,//失能电机控制(指示灯变红)
	Zero_Position_Sensor,//设定当前编码角度为零
	
}Motor_MIT_Command_e;
#endif

#define P_MIN -PI    // Radians
#define P_MAX PI        
#define V_MIN -30.0f    // Rad/s
#define V_MAX 30.0f
#define KP_MIN 0.0f     // N-m/rad
#define KP_MAX 500.0f
#define KD_MIN 0.0f     // N-m/rad/s
#define KD_MAX 5.0f
#define T_MIN -10.0f    // N.m
#define T_MAX 10.0f
#define C_MIN -10.0f    // A
#define C_MAX 10.0f


/*电机使能状态*/
typedef enum Motor_HT_Work_state_enum_e
{
	Motor_Enable,//电机可控
	Motor_Unenable,//电机不可控
	Over_Voltage,//超压
	Lack_Voltage,//欠压
	Over_Current,//过流
	MOS_OverTemp,//驱动板MOS过温
	Motor_OverTemp,//电机过温
	Commun_Loss,//通信丢失
	Unknow_Err,//未知错误
}Motor_DM_Work_state_e;


/*电机初始化参数*/
typedef struct Motor_DM_Born_Info_struct_t
{	
    uint32_t stdId;//电机控制报文ID


#ifdef __STM32F4xx_HAL_H
    CAN_HandleTypeDef *hcan;//can口选择
#endif
	
#ifdef STM32H7xx_HAL_H
    FDCAN_HandleTypeDef *hcan;//can口选择
#endif
	
	  int8_t order_correction;//扭矩正方向规定
}Motor_DM_Born_Info_t;

/*接收电机报文信息结构体*/
typedef struct Motor_DM_Rx_Info_struct_t
{
	float speed;//电机速度(单位rad/s)
	
	float torque;//电机转矩(单位N.m)
	
	float motor_angle_sum;

  float motor_angle;//电机弧度制绝对角度，-PI~PI

	float motor_angle_last;
	
	uint8_t num;//多电机发送顺序号
}Motor_DM_Rx_Info_t;

/*发送电机报文信息结构体*/
typedef struct Motor_DM_Tx_Info_struct_t
{
	float torque;//需要发送的转矩(单位N.m)
	
	float target_speed;//目标速度(单位rad/s)
	
	float target_angle;//目标角度(单位rad)
	
	float Kp;//位置增益
	
	float Kd;//速度增益
	
	uint8_t single_tx_buff[8];//使用电机发送时的个人数组
}Motor_DM_Tx_Info_t;
/* 驱动参考力矩 = (torque + Kp*err_angle + Kd*err_speed) */

/*电机状态结构体*/
typedef struct Motor_DM_State_struct_t
{
    uint32_t offline_cnt;

    uint32_t offline_cnt_max;

    dev_work_state_t status;
	
		Motor_DM_Work_state_e motor_state;
	
		Motor_DM_Work_state_e last_motor_state;
}Motor_DM_State_t;

/*单电机总结构体*/
typedef struct Motor_DM_struct_t
{
	Motor_DM_Born_Info_t* born_info;
	
	Motor_DM_Rx_Info_t* rx_info;
	
	Motor_DM_Tx_Info_t* tx_info;
	
	Motor_DM_State_t* state;


	
	void (*single_init)(struct Motor_DM_struct_t *motor);
	
	void (*single_sleep)(struct Motor_DM_struct_t *motor);
	
	void (*zero_position)(struct Motor_DM_struct_t *motor);
	
	void (*single_set_torque)(struct Motor_DM_struct_t *motor);
	
	void (*single_set_speed)(struct Motor_DM_struct_t *motor);
	
	void (*single_set_angle)(struct Motor_DM_struct_t *motor);
	
	void (*rx)(struct Motor_DM_struct_t *motor, uint8_t *rxBuf);
	
	void (*single_heart_beat)(struct Motor_DM_struct_t *motor);
}Motor_DM_t;

/*多电机结构体，由于使用MIT单电机控制，该结构体只是把单电机的一些通用的功能整合起来，方便控制，并不是真正的多电机模式*/
typedef struct Motor_DM_Group_struct_t
{
	Motor_DM_t* motor[4];
	
	uint8_t motor_num;//实际电机数量
	
	void (*group_set_torque)(struct Motor_DM_Group_struct_t *group);
	
	void (*group_sleep)(struct Motor_DM_Group_struct_t *group);
	
	void (*group_init)(struct Motor_DM_Group_struct_t *group);
	
	void (*group_heartbeat)(struct Motor_DM_Group_struct_t *group);
}Motor_DM_Group_t;

void DM_Single_Motor_Init(Motor_DM_t *motor);
void Group_Motor_Init(Motor_DM_Group_t *group);

#endif
