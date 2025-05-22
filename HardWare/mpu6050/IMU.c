#include "IMU.h"
#include "Kalman.h"
#include "math.h"

Quaternions NUB_Q = {1.0f, 0.0f, 0.0f, 0.0f}; // ��ʼ����Ԫ��
float roll, pitch, yaw;

/**
 * @brief ��Ԫ������
 *
 * @param ax
 * @param ay
 * @param az ���ٶ���������
 * @param gx
 * @param gx
 * @param gx ��������������
 * @param dt ���ڵ�����������
 */
void IMU_Updata(float ax, float ay, float az, float gx, float gy, float gz, float dt)
{
    float NormAcc;
    float vx, vy, vz;
    float ex, ey, ez;
    float kp = 10.0f;  // ����ϵ��
    float ki = 0.008f; // ����ϵ��
    float exInt = 0, eyInt = 0, ezInt = 0;

    /*��һ�����ٶ�*/
    NormAcc = 1.0f / sqrt(ax * ax + ay * ay + az * az);
    ax *= NormAcc;
    ay *= NormAcc;
    az *= NormAcc;

    /*��ȡ��������*/
    vx = 2 * (NUB_Q.q1 * NUB_Q.q3 - NUB_Q.q0 * NUB_Q.q2);
    vy = 2 * (NUB_Q.q2 * NUB_Q.q3 + NUB_Q.q0 * NUB_Q.q1);
    vz = 1 - 2 * (NUB_Q.q1 * NUB_Q.q1 + NUB_Q.q2 * NUB_Q.q2);

    /*��̬���*/
    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);

    /*������������������*/
    exInt = exInt + ex * ki * dt;
    eyInt = eyInt + ey * ki * dt;
    ezInt = ezInt + ez * ki * dt;
    /*�����������������*/
    gx = gx + kp * ex + exInt;
    gy = gy + kp * ey + eyInt;
    gz = gz + kp * ez + ezInt; // ���ڴ�����Ư

    /*һ���������,������Ԫ��*/
    NUB_Q.q0 += (-NUB_Q.q1 * gx - NUB_Q.q2 * gy - NUB_Q.q3 * gz) * 0.5f * dt;
    NUB_Q.q1 += (NUB_Q.q0 * gx + NUB_Q.q2 * gz - NUB_Q.q3 * gy) * 0.5f * dt;
    NUB_Q.q2 += (NUB_Q.q0 * gy - NUB_Q.q1 * gz + NUB_Q.q3 * gx) * 0.5f * dt;
    NUB_Q.q3 += (NUB_Q.q0 * gz + NUB_Q.q1 * gy - NUB_Q.q2 * gx) * 0.5f * dt;

    /*��һ����Ԫ��*/
    NormAcc = 1.0f / sqrt(NUB_Q.q0 * NUB_Q.q0 + NUB_Q.q1 * NUB_Q.q1 + NUB_Q.q2 * NUB_Q.q2 + NUB_Q.q3 * NUB_Q.q3);
    NUB_Q.q0 *= NormAcc;
    NUB_Q.q1 *= NormAcc;
    NUB_Q.q2 *= NormAcc;
    NUB_Q.q3 *= NormAcc;
}

/**
 * @brief ��ȡŷ����
 *
 * @param pitch ������
 * @param roll ������
 * @param yaw ƫ����
 */
void Get_Euler(float *pitch, float *roll, float *yaw)
{

    // ���㸩���� (pitch) ���� Y����ת [-90��, 90��]
    *pitch = asinf(2.0f * (NUB_Q.q0 * NUB_Q.q2 - NUB_Q.q3 * NUB_Q.q1)) * RtA;

    // �������� (roll) ���� X����ת [-180��, 180��]
    *roll = atan2f(2.0f * (NUB_Q.q0 * NUB_Q.q1 + NUB_Q.q2 * NUB_Q.q3),
                   1.0f - 2.0f * (NUB_Q.q1 * NUB_Q.q1 + NUB_Q.q2 * NUB_Q.q2)) *
            RtA;

    // ����ƫ���� (yaw) ���� Z����ת [-180��, 180��]
    *yaw = atan2f(2.0f * (NUB_Q.q0 * NUB_Q.q3 + NUB_Q.q1 * NUB_Q.q2),
                  1.0f - 2.0f * (NUB_Q.q2 * NUB_Q.q2 + NUB_Q.q3 * NUB_Q.q3)) *
           RtA;
}

/**
 * @brief ��̬����
 *
 * @param accel ���ٶ�
 * @param gyro ������
 * @param roll ������
 * @param pitch ������
 * @param yaw ƫ����
 */
void IMU(short accel[3], short gyro[3], float *roll, float *pitch, float *yaw)
{
    /*��ȡԭʼֵ�˲��������*/
    MPU6050_Filter(accel, gyro);

    float gx = (float)gyro[0] / G_LSB; // X����ٶ� (rad/s)
    float gy = (float)gyro[1] / G_LSB; // Y����ٶ� (rad/s)
    float gz = (float)gyro[2] / G_LSB; // Z����ٶ� (rad/s)

    /*������Ԫ��*/
    IMU_Updata(accel[0], accel[1], accel[2], gx, gy, gz, 0.008f);

    /*��ȡŷ����*/
    Get_Euler(pitch, roll, yaw);
}