#include "IAP.h"
#include "../HardWare/FLASH/Flash.h"

LOAD LOAD_A;
u16 IAP_Buf[1024];

/**
 * @brief
 *
 * @param addr App起始地址
 * @param buf 缓冲区
 * @param size App大小(字节)
 */
void IAP_Write(u32 addr, u8 *buf, u32 size)
{
    u16 t;
    u16 i = 0;
    u16 temp;                     // 存储转换数据
    for (t = 0; t < size; t += 2) // 循环以两单位为字节进行遍历，这是因为我们需要写入的是半字，这是因为每次给flash编程都需要写入16位数据
    {
        temp = (u16)buf[1] << 8; // 将buf转换为16位数据，并且将buf[1]移动到高8位
        temp += (u16)buf[0];     // 将buf[1]与df[0]相加，将这两个连续的字节组合成一个完整的16位字
        buf += 2;                // 偏移2个字节，准备处理下一对字节
        IAP_Buf[i++] = temp;     // 把16位的temp储存到新的数组里边
        if (i == 1024)           // flash需要块为单位进行写入操作，因此在满512个16位的数据时候，才会触发写入操作
        {
            i = 0;
            STMFLASH_Write(addr, IAP_Buf, 1024); // 进行写入
            addr += 2048;                        // 偏移1024这是为了防止下一次写入操作时，会覆盖上一个块的数据，而512个16位数据，就是512x2个字节，也就是1024
        }
    }
    if (i)
        STMFLASH_Write(addr, IAP_Buf, i); // 循环结束以后，如果IAP_Buf里边还有剩余数据，就将最后的一些内容字节写进去.
}

void IAP_LOAD(u32 addr)
{
    __disable_irq(); /*禁用全局中断，防止异常打断*/
                     /*禁用外设*/
    DMA_Cmd(DMA1_Channel5, DISABLE);
    USART_Cmd(USART1, DISABLE);
    /*清除中断标志位*/
    DMA_ClearITPendingBit(DMA1_IT_TC5);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    if (((*(vu32 *)addr) & 0x2FFE0000) == 0x20000000) // 检查栈顶地址是否合法.看其是否为ram的基地址
    {
        LOAD_A = (LOAD) * (vu32 *)(addr + 4); // 进行了两次强制转换，最终转换成了函数指针类型
        // 用户代码区第4个字节为程序开始地址(复位地址),第一个地址（0-3字节）给堆栈，第二个地址给复位向量,将APP程序的复位函数入口地址给jum2app
        MSR_MSP(*(vu32 *)addr); // 初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)，此处用汇编编写
        LOAD_A();               // 跳转到APP.
    }
}