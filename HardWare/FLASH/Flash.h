#include "sys.h"

#ifndef _FLASH_H
#define _FLASH_H

#define STM32_FLASH_SIZE 512        // ��ѡSTM32��FLASH������С(��λΪK)
#define STM_SECTOR_SIZE 2048        // һҳ��С
#define STM32_FLASH_BASE 0x08000000 // STM32 FLASH����ʼ��ַ

u16 STMFLASH_ReadHalfWord(u32 faddr);                             // ��������
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite); // ��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead);    // ��ָ����ַ��ʼ����ָ�����ȵ�����

#endif
