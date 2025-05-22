#include "sys.h"

#ifndef _KALMAN_H
#define _KALMAN_H

typedef struct
{
    float alpha;       /*滤波系数,值越大新数据权重越大(0<alpha<1)*/
    short pre_gyro[3]; /* 上一次的陀螺仪数据*/
} gyro_filter;

typedef struct
{
    float Q;          /*过程噪声协方差,表示系统模型的不确定度*/
    float R;          /*测量噪声协方差,表示传感器测量的不确定度*/
    float P;          /*估计误差协方差,反映状态估计的准确性*/
    float K;          /*卡尔曼增益,决定测量值对最终估计的影响程度*/
    short pre_acc[3]; /*上一次加速度数据*/
} acce_filter;

typedef struct
{
    float buffer[10]; // 数据缓存区
    uint8_t index;    // 当前写入位置
    float sum;        // 窗口内数据的累加和
    uint8_t count;    // 当前已存储的数据量（未填满窗口时使用）
} move_filter;

extern gyro_filter gyro_str;
extern acce_filter acce_str;
extern move_filter move_str;

void KalMan_Init(void);
void Gyro_Filter(short gyro[3]);
void Acce_Filter(short accel[3]);
float Move_Filter(float newValue);

#endif
