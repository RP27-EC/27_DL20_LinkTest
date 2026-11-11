/**
 ******************************************************************************
 * @file        imu_xrobot.h
 * @author      RobotMaster C Board
 * @brief       ATOM-IMU 成品陀螺仪 UART 通信协议层
 * 
 * 解析 IMU 数据帧，提供欧拉角、四元数、角速度、加速度等数据。
 *
 * ==== 数据获取方式 ====
 *
 * 数据在 USART1 空闲中断中接收并解析，结果存入全局变量 imu_data。
 * 只需在任意位置 extern ImuUartData imu_data; 即可读取。
 *
 * // 示例：读取欧拉角（单位：度）
 * float pitch = imu_data.eulr_.pit;
 * float roll  = imu_data.eulr_.rol;
 * float yaw   = imu_data.eulr_.yaw;
 *
 * // 示例：读取角速度（单位：rad/s）
 * float gyro_x = imu_data.gyro_.x;
 * float gyro_y = imu_data.gyro_.y;
 * float gyro_z = imu_data.gyro_.z;
 *
 * // 示例：读取加速度（单位：m/s^2）
 * float acc_x = imu_data.accl_.x;
 * float acc_y = imu_data.accl_.y;
 * float acc_z = imu_data.accl_.z;
 *
 * // 示例：读取四元数
 * float q0 = imu_data.quat_.q0;
 * float q1 = imu_data.quat_.q1;
 * float q2 = imu_data.quat_.q2;
 * float q3 = imu_data.quat_.q3;
 *
 * ==== 函数说明 ====
 *
 * IMU_UART_Init()
 *   初始化 IMU UART 数据缓存，要在 main.c 的 DEVICE_Init() 中被调用。
 *  
 *
 * CRC8_Calc(uint8_t *data, uint16_t len)
 *   CRC8 校验计算函数，内部使用，用于验证数据帧完整性。
 *
 * ==== 数据帧格式 ====
 *
 * | 偏移 | 字段     | 大小     | 说明                        |
 * |------|----------|----------|-----------------------------|
 * | 0    | prefix   | 1 字节   | 帧头，固定 0xA5             |
 * | 1    | time     | 5 字节   | 微秒时间戳 (40-bit)         |
 * | 6    | sync     | 5 字节   | 同步时间戳 (40-bit)         |
 * | 11   | quat_    | 16 字节  | 四元数 (4 * float)          |
 * | 27   | gyro_    | 12 字节  | 角速度 (3 * float, rad/s)   |
 * | 39   | accl_    | 12 字节  | 加速度 (3 * float, m/s^2)   |
 * | 51   | eulr_    | 12 字节  | 欧拉角 (3 * float, 度)      |
 * | 63   | crc8     | 1 字节   | CRC8 校验                   |
 * |      | 总计     | 64 字节  |                             |
 *
 * @note  原本欧拉角的值是弧度单位，现处理在解析的时候就转换为度，直接使用即可。
 *        陀螺仪角速度单位为 rad/s，加速度单位为 m/s^2。
 *
 * @version V1.0
 * @date    2026-06-16
 ******************************************************************************
 */

#ifndef IMU_XROBOT_H
#define IMU_XROBOT_H

#include <stdint.h>


/* Exported types ------------------------------------------------------------*/

/** @brief 三维向量 */
typedef struct __attribute__((packed)) {
    float x;
    float y;
    float z;
} Vector3;

/** @brief 四元数 */
typedef struct __attribute__((packed)) {
    float q0;
    float q1;
    float q2;
    float q3;
} Quaternion;

/** @brief 欧拉角 */
typedef struct __attribute__((packed)) {
    float rol;   // 横滚角 (度)
    float pit;   // 俯仰角 (度)
    float yaw;   // 偏航角 (度)
} EulerAngles;

/** @brief 完整 IMU 数据帧结构 */
typedef struct __attribute__((packed)) {
    uint8_t prefix;           // 帧头，固定 0xA5
    uint8_t time[5];          // 微秒时间戳 (40-bit)
    uint8_t sync[5];          // 同步时间戳 (40-bit)
    Quaternion quat_;         // 四元数
    Vector3 gyro_;            // 陀螺仪角速度 (rad/s)
    Vector3 accl_;            // 加速度 (m/s^2)
    EulerAngles eulr_;        // 欧拉角 (度)
    uint8_t crc8;             // CRC8 校验
} ImuUartData;

/* Exported variables --------------------------------------------------------*/
extern ImuUartData imu_data;

/* Exported functions --------------------------------------------------------*/
void IMU_UART_Init(void);
uint8_t CRC8_Calc(uint8_t *data, uint16_t len);

#endif
