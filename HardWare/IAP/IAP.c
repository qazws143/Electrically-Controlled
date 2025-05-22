#include "IAP.h"
#include "../HardWare/FLASH/Flash.h"

LOAD LOAD_A;
u16 IAP_Buf[1024];

/**
 * @brief
 *
 * @param addr App��ʼ��ַ
 * @param buf ������
 * @param size App��С(�ֽ�)
 */
void IAP_Write(u32 addr, u8 *buf, u32 size)
{
    u16 t;
    u16 i = 0;
    u16 temp;
    u32 fwaddr = addr; // ��ǰд��ĵ�ַ
    u8 *dfu = buf;
    for (t = 0; t < size; t += 2) // ѭ��������λΪ�ֽڽ��б�����������Ϊ������Ҫд����ǰ��֣�������Ϊÿ�θ�flash��̶���Ҫд��16λ����
    {
        temp = (u16)dfu[1] << 8; // ��dfuת��Ϊ16λ���ݣ����ҽ�dfu[1]�ƶ�����8λ
        temp += (u16)dfu[0];     // ��dfu[1]��df[0]��ӣ����������������ֽ���ϳ�һ��������16λ��
        dfu += 2;                // ƫ��2���ֽڣ�׼��������һ���ֽ�
        IAP_Buf[i++] = temp;     // ��16λ��temp���浽�µ��������
        if (i == 1024)           // flash��Ҫ��Ϊ��λ����д��������������512��16λ������ʱ�򣬲Żᴥ��д�����
        {
            i = 0;
            STMFLASH_Write(fwaddr, IAP_Buf, 1024); // ����д��
            fwaddr += 2048;                        // ƫ��1024����Ϊ�˷�ֹ��һ��д�����ʱ���Ḳ����һ��������ݣ���512��16λ���ݣ�����512x2���ֽڣ�Ҳ����1024
        }
    }
    if (i)
        STMFLASH_Write(fwaddr, IAP_Buf, i); // ѭ�������Ժ����ipabuf��߻���ʣ�����ݣ��ͽ�����һЩ�����ֽ�д��ȥ.
}

void IAP_LOAD(u32 addr)
{
    __disable_irq(); /*����ȫ���жϣ���ֹ�쳣���*/
                     /*��������*/
    DMA_Cmd(DMA1_Channel5, DISABLE);
    USART_Cmd(USART1, DISABLE);
    /*����жϱ�־λ*/
    DMA_ClearITPendingBit(DMA1_IT_TC5);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    if (((*(vu32 *)addr) & 0x2FFE0000) == 0x20000000) // ���ջ����ַ�Ƿ�Ϸ�.�����Ƿ�Ϊram�Ļ���ַ
    {
        LOAD_A = (LOAD) * (vu32 *)(addr + 4); // ����������ǿ��ת��������ת�����˺���ָ������
        // �û���������4���ֽ�Ϊ����ʼ��ַ(��λ��ַ),��һ����ַ��0-3�ֽڣ�����ջ���ڶ�����ַ����λ����	,��APP����ĸ�λ������ڵ�ַ��jum2app
        MSR_MSP(*(vu32 *)addr); // ��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)���˴��û���д
        LOAD_A();               // ��ת��APP.
    }
}