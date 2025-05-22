#include "Kalman.h"
#include "string.h"

gyro_filter gyro_str;
acce_filter acce_str;
move_filter move_str;

/**
 * @brief 角速度一阶低通滤波初始化
 *
 */
void GyroFilter_Init(void)
{
    gyro_str.alpha = 0.9f;
    for (int i = 0; i < 3; i++)
    {
        gyro_str.pre_gyro[i] = 0;
    }
}

/**
 * @brief 加速度卡尔曼滤波初始化
 *
 */
void AcceFilter_Init(void)
{
    /*可以根据需要调节Q/R值*/
    acce_str.Q = 0.003f; // 过程噪声，Q越大，滤波器越信任测量值
    acce_str.R = 0.1f;   // 测量噪声，R越大，滤波器越信任预测值
    acce_str.P = 1.0f;   // 初始估计误差协方差，通常设为1.0，表示初始状态的不确定性较高
    acce_str.K = 0.0f;   // 卡尔曼增益初始化为0，表示还未开始
    for (int i = 0; i < 3; i++)
    {
        acce_str.pre_acc[i] = 0;
    }
}

/**
 * @brief 移动平均滤波
 *
 */
void MovingFilter_Init(void)
{
    memset(move_str.buffer, 0, sizeof(move_str.buffer)); // 清零缓存
    move_str.index = 0;
    move_str.sum = 0;
    move_str.count = 0;
}

void KalMan_Init(void)
{
    AcceFilter_Init();
    GyroFilter_Init();
    MovingFilter_Init();
}

/**
 * @brief 一阶低通滤波计算
 *
 * @param gyro 原始数据,滤波后直接覆盖
 */
void Gyro_Filter(short gyro[3])
{
    for (int i = 0; i < 3; i++)
    {
        /*滤波后数据=滤波系数*当前测量值+(1-滤波系数)*上一次测量值*/
        gyro[i] = (short)(gyro_str.alpha * gyro[i] + (1.0f - gyro_str.alpha) * gyro_str.pre_gyro[i]);
        gyro_str.pre_gyro[i] = gyro[i];
    }
}

/**
 * @brief 卡尔曼滤波
 *
 * @param accel 原始数据,滤波后直接覆盖
 */
void Acce_Filter(short accel[3])
{
    for (int i = 0; i < 3; i++)
    {
        acce_str.P += acce_str.Q;                                     // 预测当前状态的误差协方差,随着时间推移系统不确定性增加
        acce_str.K = acce_str.P / (acce_str.P + acce_str.R);          // 计算卡尔曼增益k=P/(P+R)
        short filter = (short)(acce_str.pre_acc[i] +                  // 修正上一时刻的测量值
                               acce_str.K *                           // 根据卡尔曼增益修正
                                   (accel[i] - acce_str.pre_acc[i])); // 当前测量值与上一测量值的差异
        acce_str.P *= (1 - acce_str.K);                               // 通过新测量值降低不确定性
        acce_str.pre_acc[i] = filter;                                 // 更新滤波值
        accel[i] = filter;
    }
}

/**
 * @brief 移动平均滤波
 *
 * @param 计算数据
 * @return 滤波后数据
 */
float Move_Filter(float newValue)
{
    // 移除旧数据：从总和中减去即将被覆盖的值
    move_str.sum -= move_str.buffer[move_str.index];

    // 添加新数据：更新缓冲区并累加到总和
    move_str.buffer[move_str.index] = newValue;
    move_str.sum += newValue;

    // 更新环形缓冲区索引
    move_str.index = (move_str.index + 1) % 5;

    // 处理初始未填满窗口的情况
    if (move_str.count < 5)
    {
        move_str.count++;
    }

    // 返回当前平均值
    return move_str.sum / move_str.count;
}