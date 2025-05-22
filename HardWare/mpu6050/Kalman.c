#include "Kalman.h"
#include "string.h"

gyro_filter gyro_str;
acce_filter acce_str;
move_filter move_str;

/**
 * @brief ���ٶ�һ�׵�ͨ�˲���ʼ��
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
 * @brief ���ٶȿ������˲���ʼ��
 *
 */
void AcceFilter_Init(void)
{
    /*���Ը�����Ҫ����Q/Rֵ*/
    acce_str.Q = 0.003f; // ����������QԽ���˲���Խ���β���ֵ
    acce_str.R = 0.1f;   // ����������RԽ���˲���Խ����Ԥ��ֵ
    acce_str.P = 1.0f;   // ��ʼ�������Э���ͨ����Ϊ1.0����ʾ��ʼ״̬�Ĳ�ȷ���Խϸ�
    acce_str.K = 0.0f;   // �����������ʼ��Ϊ0����ʾ��δ��ʼ
    for (int i = 0; i < 3; i++)
    {
        acce_str.pre_acc[i] = 0;
    }
}

/**
 * @brief �ƶ�ƽ���˲�
 *
 */
void MovingFilter_Init(void)
{
    memset(move_str.buffer, 0, sizeof(move_str.buffer)); // ���㻺��
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
 * @brief һ�׵�ͨ�˲�����
 *
 * @param gyro ԭʼ����,�˲���ֱ�Ӹ���
 */
void Gyro_Filter(short gyro[3])
{
    for (int i = 0; i < 3; i++)
    {
        /*�˲�������=�˲�ϵ��*��ǰ����ֵ+(1-�˲�ϵ��)*��һ�β���ֵ*/
        gyro[i] = (short)(gyro_str.alpha * gyro[i] + (1.0f - gyro_str.alpha) * gyro_str.pre_gyro[i]);
        gyro_str.pre_gyro[i] = gyro[i];
    }
}

/**
 * @brief �������˲�
 *
 * @param accel ԭʼ����,�˲���ֱ�Ӹ���
 */
void Acce_Filter(short accel[3])
{
    for (int i = 0; i < 3; i++)
    {
        acce_str.P += acce_str.Q;                                     // Ԥ�⵱ǰ״̬�����Э����,����ʱ������ϵͳ��ȷ��������
        acce_str.K = acce_str.P / (acce_str.P + acce_str.R);          // ���㿨��������k=P/(P+R)
        short filter = (short)(acce_str.pre_acc[i] +                  // ������һʱ�̵Ĳ���ֵ
                               acce_str.K *                           // ���ݿ�������������
                                   (accel[i] - acce_str.pre_acc[i])); // ��ǰ����ֵ����һ����ֵ�Ĳ���
        acce_str.P *= (1 - acce_str.K);                               // ͨ���²���ֵ���Ͳ�ȷ����
        acce_str.pre_acc[i] = filter;                                 // �����˲�ֵ
        accel[i] = filter;
    }
}

/**
 * @brief �ƶ�ƽ���˲�
 *
 * @param ��������
 * @return �˲�������
 */
float Move_Filter(float newValue)
{
    // �Ƴ������ݣ����ܺ��м�ȥ���������ǵ�ֵ
    move_str.sum -= move_str.buffer[move_str.index];

    // ��������ݣ����»��������ۼӵ��ܺ�
    move_str.buffer[move_str.index] = newValue;
    move_str.sum += newValue;

    // ���»��λ���������
    move_str.index = (move_str.index + 1) % 5;

    // �����ʼδ�������ڵ����
    if (move_str.count < 5)
    {
        move_str.count++;
    }

    // ���ص�ǰƽ��ֵ
    return move_str.sum / move_str.count;
}