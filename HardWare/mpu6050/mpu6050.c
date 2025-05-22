#include "../HardWare/mpu6050/mpu6050.h"
#include "I2C.h"
#include "delay.h"
#include "Kalman.h"

short Acel[3];
short Gyro[3];

/**
 * @brief 写寄存器
 *
 * @param reg_add 读取地址
 * @param reg_dat 读取数据
 */
void MPU6050_WriteReg(u8 reg_add, u8 reg_dat)
{
    I2C_ByteWrite(reg_dat, reg_add);
}

/**
 * @brief 读寄存器
 *
 * @param reg_add 写入地址
 * @param Read 读取数据
 * @param num 读取长度
 */
void MPU6050_ReadData(u8 reg_add, unsigned char *Read, u8 num)
{
    I2C_BufferRead(Read, reg_add, num);
}

/**
 * @brief MPU6050初始化
 *
 */
void MPU6050_Init(void)
{
    int res = 0;
    // 在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
    Delay_ms(100);
    I2C_Config();
    MPU6050_WriteReg(MPU_PWR_MGMT1_REG, 0x00);   // 解除休眠状态
    MPU6050_WriteReg(MPU_USER_CTRL_REG, 0X00);   // I2C主模式关闭
    MPU6050_WriteReg(MPU_INTBP_CFG_REG, 0X80);   // 设置MPU6050中断
    MPU6050_WriteReg(MPU_SAMPLE_RATE_REG, 0x13); // 陀螺仪采样率50Hz
    MPU6050_WriteReg(MPU_CFG_REG, 0x06);         // 数字低通滤波器配置为5Hz带宽
    MPU6050_WriteReg(MPU_ACCEL_CFG_REG, 0x01);   // 配置加速度传感器工作在±2g量程
    MPU6050_WriteReg(MPU_GYRO_CFG_REG, 0x18);    // 陀螺仪自检及测量范围(2000deg/s)
    res = MPU6050_ReadID();
    if (res)
    {
        MPU6050_WriteReg(MPU_PWR_MGMT1_REG, 0X01); // 设置CLKSEL,PLL X轴为参考
        MPU6050_WriteReg(MPU_PWR_MGMT2_REG, 0X00); // 加速度与陀螺仪都工作
    }
}

/**
 * @brief   读取MPU6050的ID
 * @param
 * @retval  正常返回1，异常返回0
 */
uint8_t MPU6050_ReadID(void)
{
    unsigned char Re = 0;
    MPU6050_ReadData(MPU_DEVICE_ID_REG, &Re, 1); // 读器件地址
    if (Re != 0x68)
        return 0;
    else
        return 1;
}

/**
 * @brief 读取加速度
 *
 * @param accData 接收读取数据
 */
void MPU6050_ReadAcc(short *accData)
{
    u8 buf[6];
    MPU6050_ReadData(MPU_ACCEL_XOUTH_REG, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

/**
 * @brief 读取角速度
 *
 * @param gyroData 接收角速度数据
 */
void MPU6050_ReadGyro(short *gyroData)
{
    u8 buf[6];
    MPU6050_ReadData(MPU_GYRO_XOUTH_REG, buf, 6);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}

/**
 * @brief 读取原始温度
 *
 * @param tempData 存储温度
 */
void MPU6050_ReadTemp(short *tempData)
{
    u8 buf[2];
    MPU6050_ReadData(MPU_TEMP_OUTH_REG, buf, 2); // 读取温度值
    *tempData = (buf[0] << 8) | buf[1];
}

/**
 * @brief 转化为摄氏度
 *
 * @param Temperature 接收温度
 */
void MPU6050_ReturnTemp(short *Temperature)
{
    short temp3;
    u8 buf[2];

    MPU6050_ReadData(MPU_TEMP_OUTH_REG, buf, 2); // 读取温度值
    temp3 = (buf[0] << 8) | buf[1];
    *Temperature = ((double)temp3 / 340.0) + 36.53;
}

/**
 * @brief 原始数据
 *
 * @param accel 加速度
 * @param gyro 角速度
 */
void MPU6050_Raw_Read(short accel[3], short gyro[3])
{
    MPU6050_ReadAcc(accel);
    MPU6050_ReadGyro(gyro);
}

/**
 * @brief 原始数据滤波
 *
 * @param accel 加速度
 * @param gyro 角速度
 */
void MPU6050_Filter(short accel[3], short gyro[3])
{
    MPU6050_Raw_Read(accel, gyro);
    Gyro_Filter(gyro);
    Acce_Filter(accel);
}
