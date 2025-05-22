#include "sys.h"

#ifndef _HCSR04_H
#define _HCSR04_H

#define HCSR04_CLK RCC_APB2Periph_GPIOA
#define HCSR04_PORT GPIOA
#define HCSR04_TRIG GPIO_Pin_11
#define HCSR04_ECHO GPIO_Pin_8

typedef struct
{
    u8 capStartFlag; // ����ʼ��־
    u8 capEndFlag;   // ���������־
    u16 capValue;    // �Զ���װ��ֵ
    u16 capPeriods;  // �Զ���װ�ؼ������������
} capflag;

extern capflag HCSR04_Flag;

void HCSR04_GetLen(float *distance);
void HCSR04_Init(void);
void HCSR04_Start(void);

#endif
