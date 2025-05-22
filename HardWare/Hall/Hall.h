#include "sys.h"

#ifndef _HALL_H
#define _HALL_H

#define HALL_CLK RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1
#define HALL_PORT GPIOC
#define HALL_PIN GPIO_Pin_1
#define HALL_ADC ADC1

void Hall_Init(void);
u16 Get_ADC(void);

#endif
