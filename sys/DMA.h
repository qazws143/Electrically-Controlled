#include "sys.h"

#ifndef _DMA_H
#define _DMA_H

#define buff_size 4096
void DMA_Config(u32 cpar, u32 cmar, u16 cndtr);
void DMA_Enable(void);
uint16_t GetReceivedDataLength(void);
#endif
