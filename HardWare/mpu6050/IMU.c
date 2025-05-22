#include "IMU.h"
#include "Kalman.h"
#include "math.h"

Quaternions NUB_Q = {1.0f, 0.0f, 0.0f, 0.0f}; // 初始化四元数
float roll, pitch, yaw;

/**
 * @brief 四元数更新
 *
 * @param ax
 * @param ay
 * @param az 加速度三轴数据
 * @param gx
 * @param gx
 * @param gx 陀螺仪三轴数据
 * @param dt 用于调整采样速率
 */
void IMU_Updata(float ax, float ay, float az, float gx, float gy, float gz, float dt)
{
    float NormAcc;
    float vx, vy, vz;
    float ex, ey, ez;
    float kp = 10.0f;  // 比例系数
    float ki = 0.008f; // 积分系数
    float exInt = 0, eyInt = 0, ezInt = 0;

    /*归一化加速度*/
    NormAcc = 1.0f / sqrt(ax * ax + ay * ay + az * az);
    ax *= NormAcc;
    ay *= NormAcc;
    az *= NormAcc;

    /*提取重力分量*/
    vx = 2 * (NUB_Q.q1 * NUB_Q.q3 - NUB_Q.q0 * NUB_Q.q2);
    vy = 2 * (NUB_Q.q2 * NUB_Q.q3 + NUB_Q.q0 * NUB_Q.q1);
    vz = 1 - 2 * (NUB_Q.q1 * NUB_Q.q1 + NUB_Q.q2 * NUB_Q.q2);

    /*姿态误差*/
    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);

    /*积分误差比例积分增益*/
    exInt = exInt + ex * ki * dt;
    eyInt = eyInt + ey * ki * dt;
    ezInt = ezInt + ez * ki * dt;
    /*调整后的陀螺仪数据*/
    gx = gx + kp * ex + exInt;
    gy = gy + kp * ey + eyInt;
    gz = gz + kp * ez + ezInt; // 用于处理零漂

    /*一阶龙格库塔,更新四元数*/
    NUB_Q.q0 += (-NUB_Q.q1 * gx - NUB_Q.q2 * gy - NUB_Q.q3 * gz) * 0.5f * dt;
    NUB_Q.q1 += (NUB_Q.q0 * gx + NUB_Q.q2 * gz - NUB_Q.q3 * gy) * 0.5f * dt;
    NUB_Q.q2 += (NUB_Q.q0 * gy - NUB_Q.q1 * gz + NUB_Q.q3 * gx) * 0.5f * dt;
    NUB_Q.q3 += (NUB_Q.q0 * gz + NUB_Q.q1 * gy - NUB_Q.q2 * gx) * 0.5f * dt;

    /*归一化四元数*/
    NormAcc = 1.0f / sqrt(NUB_Q.q0 * NUB_Q.q0 + NUB_Q.q1 * NUB_Q.q1 + NUB_Q.q2 * NUB_Q.q2 + NUB_Q.q3 * NUB_Q.q3);
    NUB_Q.q0 *= NormAcc;
    NUB_Q.q1 *= NormAcc;
    NUB_Q.q2 *= NormAcc;
    NUB_Q.q3 *= NormAcc;
}

/**
 * @brief 获取欧拉角
 *
 * @param pitch 俯仰角
 * @param roll 滚动角
 * @param yaw 偏航角
 */
void Get_Euler(float *pitch, float *roll, float *yaw)
{

    // 计算俯仰角 (pitch) —— Y轴旋转 [-90°, 90°]
    *pitch = asinf(2.0f * (NUB_Q.q0 * NUB_Q.q2 - NUB_Q.q3 * NUB_Q.q1)) * RtA;

    // 计算横滚角 (roll) —— X轴旋转 [-180°, 180°]
    *roll = atan2f(2.0f * (NUB_Q.q0 * NUB_Q.q1 + NUB_Q.q2 * NUB_Q.q3),
                   1.0f - 2.0f * (NUB_Q.q1 * NUB_Q.q1 + NUB_Q.q2 * NUB_Q.q2)) *
            RtA;

    // 计算偏航角 (yaw) —— Z轴旋转 [-180°, 180°]
    *yaw = atan2f(2.0f * (NUB_Q.q0 * NUB_Q.q3 + NUB_Q.q1 * NUB_Q.q2),
                  1.0f - 2.0f * (NUB_Q.q2 * NUB_Q.q2 + NUB_Q.q3 * NUB_Q.q3)) *
           RtA;
}

/**
 * @brief 姿态解算
 *
 * @param accel 加速度
 * @param gyro 陀螺仪
 * @param roll 滚动角
 * @param pitch 俯仰角
 * @param yaw 偏航角
 */
void IMU(short accel[3], short gyro[3], float *roll, float *pitch, float *yaw)
{
    /*获取原始值滤波后的数据*/
    MPU6050_Filter(accel, gyro);

    float gx = (float)gyro[0] / G_LSB; // X轴角速度 (rad/s)
    float gy = (float)gyro[1] / G_LSB; // Y轴角速度 (rad/s)
    float gz = (float)gyro[2] / G_LSB; // Z轴角速度 (rad/s)

    /*更新四元数*/
    IMU_Updata(accel[0], accel[1], accel[2], gx, gy, gz, 0.008f);

    /*获取欧拉角*/
    Get_Euler(pitch, roll, yaw);
}