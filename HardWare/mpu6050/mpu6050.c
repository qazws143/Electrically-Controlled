#include "../HardWare/mpu6050/mpu6050.h"
#include "I2C.h"
#include "delay.h"
#include "Kalman.h"

short Acel[3];
short Gyro[3];

/**
 * @brief д�Ĵ���
 *
 * @param reg_add ��ȡ��ַ
 * @param reg_dat ��ȡ����
 */
void MPU6050_WriteReg(u8 reg_add, u8 reg_dat)
{
    I2C_ByteWrite(reg_dat, reg_add);
}

/**
 * @brief ���Ĵ���
 *
 * @param reg_add д���ַ
 * @param Read ��ȡ����
 * @param num ��ȡ����
 */
void MPU6050_ReadData(u8 reg_add, unsigned char *Read, u8 num)
{
    I2C_BufferRead(Read, reg_add, num);
}

/**
 * @brief MPU6050��ʼ��
 *
 */
void MPU6050_Init(void)
{
    int res = 0;
    // �ڳ�ʼ��֮ǰҪ��ʱһ��ʱ�䣬��û����ʱ����ϵ�����ϵ����ݿ��ܻ����
    Delay_ms(100);
    I2C_Config();
    MPU6050_WriteReg(MPU_PWR_MGMT1_REG, 0x00);   // �������״̬
    MPU6050_WriteReg(MPU_USER_CTRL_REG, 0X00);   // I2C��ģʽ�ر�
    MPU6050_WriteReg(MPU_INTBP_CFG_REG, 0X80);   // ����MPU6050�ж�
    MPU6050_WriteReg(MPU_SAMPLE_RATE_REG, 0x13); // �����ǲ�����50Hz
    MPU6050_WriteReg(MPU_CFG_REG, 0x06);         // ���ֵ�ͨ�˲�������Ϊ5Hz����
    MPU6050_WriteReg(MPU_ACCEL_CFG_REG, 0x01);   // ���ü��ٶȴ����������ڡ�2g����
    MPU6050_WriteReg(MPU_GYRO_CFG_REG, 0x18);    // �������Լ켰������Χ(2000deg/s)
    res = MPU6050_ReadID();
    if (res)
    {
        MPU6050_WriteReg(MPU_PWR_MGMT1_REG, 0X01); // ����CLKSEL,PLL X��Ϊ�ο�
        MPU6050_WriteReg(MPU_PWR_MGMT2_REG, 0X00); // ���ٶ��������Ƕ�����
    }
}

/**
 * @brief   ��ȡMPU6050��ID
 * @param
 * @retval  ��������1���쳣����0
 */
uint8_t MPU6050_ReadID(void)
{
    unsigned char Re = 0;
    MPU6050_ReadData(MPU_DEVICE_ID_REG, &Re, 1); // ��������ַ
    if (Re != 0x68)
        return 0;
    else
        return 1;
}

/**
 * @brief ��ȡ���ٶ�
 *
 * @param accData ���ն�ȡ����
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
 * @brief ��ȡ���ٶ�
 *
 * @param gyroData ���ս��ٶ�����
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
 * @brief ��ȡԭʼ�¶�
 *
 * @param tempData �洢�¶�
 */
void MPU6050_ReadTemp(short *tempData)
{
    u8 buf[2];
    MPU6050_ReadData(MPU_TEMP_OUTH_REG, buf, 2); // ��ȡ�¶�ֵ
    *tempData = (buf[0] << 8) | buf[1];
}

/**
 * @brief ת��Ϊ���϶�
 *
 * @param Temperature �����¶�
 */
void MPU6050_ReturnTemp(short *Temperature)
{
    short temp3;
    u8 buf[2];

    MPU6050_ReadData(MPU_TEMP_OUTH_REG, buf, 2); // ��ȡ�¶�ֵ
    temp3 = (buf[0] << 8) | buf[1];
    *Temperature = ((double)temp3 / 340.0) + 36.53;
}

/**
 * @brief ԭʼ����
 *
 * @param accel ���ٶ�
 * @param gyro ���ٶ�
 */
void MPU6050_Raw_Read(short accel[3], short gyro[3])
{
    MPU6050_ReadAcc(accel);
    MPU6050_ReadGyro(gyro);
}

/**
 * @brief ԭʼ�����˲�
 *
 * @param accel ���ٶ�
 * @param gyro ���ٶ�
 */
void MPU6050_Filter(short accel[3], short gyro[3])
{
    MPU6050_Raw_Read(accel, gyro);
    Gyro_Filter(gyro);
    Acce_Filter(accel);
}
