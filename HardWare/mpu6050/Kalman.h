#include "sys.h"

#ifndef _KALMAN_H
#define _KALMAN_H

typedef struct
{
    float alpha;       /*�˲�ϵ��,ֵԽ��������Ȩ��Խ��(0<alpha<1)*/
    short pre_gyro[3]; /* ��һ�ε�����������*/
} gyro_filter;

typedef struct
{
    float Q;          /*��������Э����,��ʾϵͳģ�͵Ĳ�ȷ����*/
    float R;          /*��������Э����,��ʾ�����������Ĳ�ȷ����*/
    float P;          /*�������Э����,��ӳ״̬���Ƶ�׼ȷ��*/
    float K;          /*����������,��������ֵ�����չ��Ƶ�Ӱ��̶�*/
    short pre_acc[3]; /*��һ�μ��ٶ�����*/
} acce_filter;

typedef struct
{
    float buffer[10]; // ���ݻ�����
    uint8_t index;    // ��ǰд��λ��
    float sum;        // ���������ݵ��ۼӺ�
    uint8_t count;    // ��ǰ�Ѵ洢����������δ��������ʱʹ�ã�
} move_filter;

extern gyro_filter gyro_str;
extern acce_filter acce_str;
extern move_filter move_str;

void KalMan_Init(void);
void Gyro_Filter(short gyro[3]);
void Acce_Filter(short accel[3]);
float Move_Filter(float newValue);

#endif
