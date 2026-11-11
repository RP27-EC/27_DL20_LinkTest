/**
 * @file        rp_math.h
 * @author      RobotPilots
 * @Version     v1.1
 * @brief       RobotPilots Robots' Math Libaray.
 * @update
 *              v1.0(11-September-2020)
 *              v1.1(13-November-2021)
 *                  1.增加位操作函数
 */

#ifndef __RP_MATH_H
#define __RP_MATH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported macro ------------------------------------------------------------*/
#define ANGLE_TO_RAD 0.01745f

typedef enum jugde_logical_e // 逻辑判断
{
	Flase = 0,
	True = 1,

} jugde_logical_e;

typedef struct Time_trigger_struct{
	uint8_t *private_flag;
	uint32_t *last_trigger_tick;
	uint8_t *ignore_first_trigger_flag; 
	uint8_t flag_before_trigger;
	uint8_t flag_after_trigger;
	uint32_t delay_tick;
	uint8_t if_ignore_first;
}Time_trigger_t;
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* 位操作函数 */
#define SET_EVENT(EVENT, FLAG) ((EVENT) |= FLAG)
#define CLEAR_EVENT(EVENT, FLAG) ((EVENT) &= ~(FLAG))
#define GET_EVENT(EVENT, FLAG) ((EVENT) & (FLAG))
/* 数值函数 */
#define constrain(x, min, max) ((x > max) ? max : (x < min ? min : x))			  // 如果x在范围内就返回x，否则返回min或max
#define max(a, b) ((a) > (b) ? (a) : (b))										  // 返回大的
#define min(a, b) ((a) < (b) ? (a) : (b))										  // 返回小的
#define abs(x) ((x) > 0 ? (x) : (-(x)))											  // 返回绝对值
#define one(x) ((x) > 0 ? (1) : (-1))											  // 如果>0就返回1，否则返回-1
#define sgn(x) (((x) > 0) ? 1 : ((x) < 0 ? -1 : 0))								  // 返回符号
#define within_or_not(x, min, max) (x > max) ? Flase : ((x < min) ? Flase : True) // 在规定范围外返回0
#define angle2rad(ANGLE) ((ANGLE) * ANGLE_TO_RAD)

/* 斜坡函数 */
int16_t RampInt(int16_t final, int16_t now, int16_t ramp);
float RampFloat(float final, float now, float ramp);
/* 死区函数 */
float DeathZoom(float input, float center, float death);
/* 低通滤波 */
float Lowpass(float X_last, float X_new, float K);
/* 半圈处理 */
float motor_half_cycle(float angle, float max);

#endif
