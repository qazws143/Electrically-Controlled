#include "sys.h"

#ifndef _FLASH_H
#define _FLASH_H

#define STM32_FLASH_SIZE 512        // 所选STM32的FLASH容量大小(单位为K)
#define STM_SECTOR_SIZE 2048        // 一页大小
#define STM32_FLASH_BASE 0x08000000 // STM32 FLASH的起始地址

u16 STMFLASH_ReadHalfWord(u32 faddr);                             // 读出半字
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite); // 从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead);    // 从指定地址开始读出指定长度的数据

#endif
