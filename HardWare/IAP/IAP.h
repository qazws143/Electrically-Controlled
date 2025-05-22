#include "sys.h"

#ifndef _IAP_H
#define _IAP_H

typedef void (*LOAD)(void);
#define FLASH_APP1_ADDR 0x08007800 // ��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)

void IAP_Write(u32 addr, u8 *buf, u32 size);
void IAP_LOAD(u32 addr);
#endif
