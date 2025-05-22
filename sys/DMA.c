#include "DMA.h"
#include "bsp_usart.h"

u16 DMA1_MEM_LEN; // 保存DMA每次数据传送的长度

/**
 * @brief DMA初始化
 *
 */
void DMA_Config(u32 cpar, u32 cmar, u16 cndtr)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 使能DMA传输
    DMA_DeInit(DMA1_Channel5);                         // 将DMA的通道1寄存器重设为缺省值
    DMA1_MEM_LEN = cndtr;
    DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;                        // DMA外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = cmar;                            // DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      // 数据传输方向，从外设到内存
    DMA_InitStructure.DMA_BufferSize = cndtr;                               // DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 数据宽度为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 数据宽度为8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 工作在正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     // DMA通道 x拥有高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);                            // 根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;  // DMA1通道5中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断
    NVIC_Init(&NVIC_InitStructure);

    // 开启DMA传输完成中断功能
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

    DMA_Cmd(DMA1_Channel5, ENABLE); // 使能后，开启一次DMA传输
}

/**
 * @brief
 *
 */
void DMA_Enable(void)
{
    DMA_Cmd(DMA1_Channel5, DISABLE);                     // 关闭USART1 RX DMA1 所指示的通道
    DMA_SetCurrDataCounter(DMA1_Channel5, DMA1_MEM_LEN); // DMA通道的DMA缓存的大小
    DMA_Cmd(DMA1_Channel5, ENABLE);                      // 使能USART1 RX DMA1 所指示的通道
}

uint16_t GetReceivedDataLength(void)
{
    // 计算已接收的数据量
    return buff_size - DMA_GetCurrDataCounter(DMA1_Channel5);
}