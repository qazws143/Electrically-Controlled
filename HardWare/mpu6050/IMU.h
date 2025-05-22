#include "sys.h"
#include "mpu6050.h"

#ifndef _IMU_H
#define _IMU_H

#define G_LSB 16.4f * 0.0174533f

#define RtA 57.2957795f // 弧度转角度

/*定义四元数*/
typedef struct
{
    float q0;
    float q1;
    float q2;
    float q3;
} Quaternions;

extern float roll, pitch, yaw;

void IMU_Updata(float ax, float ay, float az, float gx, float gy, float gz, float dt);
void Get_Euler(float *pitch, float *roll, float *yaw);
void IMU(short accel[3], short gyro[3], float *roll, float *pitch, float *yaw);

#endif
