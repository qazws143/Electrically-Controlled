#include "DMA.h"
#include "bsp_usart.h"

u16 DMA1_MEM_LEN; // ����DMAÿ�����ݴ��͵ĳ���

/**
 * @brief DMA��ʼ��
 *
 */
void DMA_Config(u32 cpar, u32 cmar, u16 cndtr)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ʹ��DMA����
    DMA_DeInit(DMA1_Channel5);                         // ��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
    DMA1_MEM_LEN = cndtr;
    DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;                        // DMA�������ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = cmar;                            // DMA�ڴ����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      // ���ݴ��䷽�򣬴����赽�ڴ�
    DMA_InitStructure.DMA_BufferSize = cndtr;                               // DMAͨ����DMA����Ĵ�С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�Ĵ�������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ��ַ�Ĵ�������
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ���ݿ��Ϊ8λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // ���ݿ��Ϊ8λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ����������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     // DMAͨ�� xӵ�и����ȼ�
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);                            // ����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;  // DMA1ͨ��5�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; // ��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // �����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // ʹ���ж�
    NVIC_Init(&NVIC_InitStructure);

    // ����DMA��������жϹ���
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

    DMA_Cmd(DMA1_Channel5, ENABLE); // ʹ�ܺ󣬿���һ��DMA����
}

/**
 * @brief
 *
 */
void DMA_Enable(void)
{
    DMA_Cmd(DMA1_Channel5, DISABLE);                     // �ر�USART1 RX DMA1 ��ָʾ��ͨ��
    DMA_SetCurrDataCounter(DMA1_Channel5, DMA1_MEM_LEN); // DMAͨ����DMA����Ĵ�С
    DMA_Cmd(DMA1_Channel5, ENABLE);                      // ʹ��USART1 RX DMA1 ��ָʾ��ͨ��
}

uint16_t GetReceivedDataLength(void)
{
    // �����ѽ��յ�������
    return buff_size - DMA_GetCurrDataCounter(DMA1_Channel5);
}