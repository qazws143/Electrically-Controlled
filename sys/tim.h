#include "sys.h"

#ifndef _TIM_H
#define _TIM_H

#define TIM_CLK RCC_APB1Periph_TIM2
#define GPIO_CLK RCC_APB2Periph_GPIOA
#define TIM_PORT TIM2
#define GPIO_PORT GPIOA

void TIM2_Init(u16 arr, u16 psc);
void Set_Servo(TIM_TypeDef *TIM, u8 Channel, u8 duty);

#endif
