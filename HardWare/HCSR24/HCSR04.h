#include "sys.h"

#ifndef _HCSR04_H
#define _HCSR04_H

#define HCSR04_CLK RCC_APB2Periph_GPIOA
#define HCSR04_PORT GPIOA
#define HCSR04_TRIG GPIO_Pin_11
#define HCSR04_ECHO GPIO_Pin_8

typedef struct
{
    u8 capStartFlag; // 捕获开始标志
    u8 capEndFlag;   // 捕获结束标志
    u16 capValue;    // 自动重装载值
    u16 capPeriods;  // 自动重装载计数器溢出次数
} capflag;

extern capflag HCSR04_Flag;

void HCSR04_GetLen(float *distance);
void HCSR04_Init(void);
void HCSR04_Start(void);

#endif
