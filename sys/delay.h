#ifndef __DELAY_H
#define __DELAY_H
#include "stm32f10x.h"

void Delay_init();
void Delay_us(uint16_t us);
void Delay_ms(uint16_t ms);
void Delay_xms(uint16_t ms);

#endif
