#include "I2C.h"
#include "../HardWare/mpu6050/mpu6050.h"

/* ȫ�ֱ������� */
static __IO uint32_t I2CTimeout = I2CT_LONG_TIMEOUT; // I2C������ʱ������

// ����������Ҫ����ȴ�ʱ�䣬�������׶�ȡʧ��

/**
 * @brief  I2C��ʼ��
 * @note   ����I2CΪ400kHz��׼ģʽ��7λ��ַģʽ
 * @param  ��
 * @retval ��
 */
void I2C_Config(void)
{
    I2C_InitTypeDef I2C_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ʹ��I2C����ʱ�Ӻ�GPIOʱ�� */
    SENSORS_I2C_APBxClock_FUN(SENSORS_I2C_CLK, ENABLE);
    SENSORS_I2C_GPIO_APBxClock_FUN(SENSORS_I2C_GPIO_CLK, ENABLE);

    /* ����SCL����(PB6) */
    GPIO_InitStructure.GPIO_Pin = SENSORS_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // ����ģʽ
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;   // ���ÿ�©���
    GPIO_Init(SENSORS_I2C_SCL_PORT, &GPIO_InitStructure);

    /* ����SDA����(PB7) */
    GPIO_InitStructure.GPIO_Pin = SENSORS_I2C_SDA_PIN;
    GPIO_Init(SENSORS_I2C_SDA_PORT, &GPIO_InitStructure);

    /* I2C ������������ */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                // I2Cģʽ
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                        // Tlow/Thigh = 2
    I2C_InitStructure.I2C_OwnAddress1 = 0xff;                                 // ������ַ(δʹ��)
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                               // ʹ��ACK��Ӧ
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7λ��ַģʽ
    I2C_InitStructure.I2C_ClockSpeed = 400000;                                // 400kHz��׼ģʽ

    /*DMA��ȡ����*/

    /* ��ʼ��I2C */
    I2C_Init(SENSORS_I2Cx, &I2C_InitStructure);

    /* ʹ��I2C */
    I2C_Cmd(SENSORS_I2Cx, ENABLE);
}

/**
 * @brief  ��I2C�豸д��һ���ֽ�����
 * @param  pBuffer: Ҫд�������
 * @param  WriteAddr: ���豸�ڲ��Ĵ�����ַ
 * @retval 1: д��ɹ� 0: д��ʧ��(��ʱ)
 */
uint8_t I2C_ByteWrite(u8 pBuffer, u8 WriteAddr)
{
    uint32_t timeout = I2CT_LONG_TIMEOUT;
    I2C_GenerateSTART(SENSORS_I2Cx, ENABLE);

    /* �ȴ�EV5�¼�(����ģʽ��ѡ��) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* ���ʹ��豸��ַ(дģʽ) */
    I2C_Send7bitAddress(SENSORS_I2Cx, MPU_ADDR, I2C_Direction_Transmitter);

    /* �ȴ�EV6�¼�(��������ģʽ��ѡ��) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* ����Ҫ��ȡ�ļĴ�����ַ */
    I2C_SendData(SENSORS_I2Cx, WriteAddr);

    /* �ȴ�EV8�¼�(��ַ�ֽ��ѷ���) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((timeout--) == 0)
            return 0;
    }
    /* ����Ҫд������� */
    I2C_SendData(SENSORS_I2Cx, pBuffer);

    /* �ȴ�EV8�¼�(�����ֽ��ѷ���) */
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((timeout--) == 0)
            return 0;
    }
    /* ����STOP���� */
    I2C_GenerateSTOP(SENSORS_I2Cx, ENABLE);

    return 1; // д��ɹ�
}

/**
 * @brief  ��I2C�豸��ȡ����ֽ�����
 * @param  pBuffer: ���ݴ洢������ָ��
 * @param  ReadAddr: ���豸�ڲ���ʼ�Ĵ�����ַ
 * @param  NumByteToRead: Ҫ��ȡ���ֽ���
 * @retval 1: ��ȡ�ɹ� 0: ��ȡʧ��(��ʱ)
 *
 */
uint8_t I2C_BufferRead(u8 *pBuffer, u8 ReadAddr, u16 NumByteToRead)
{
    /* �ȴ����߿��� */
    uint32_t timeout = I2CT_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(SENSORS_I2Cx, I2C_FLAG_BUSY))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* ����START���� */
    I2C_GenerateSTART(SENSORS_I2Cx, ENABLE);

    /* �ȴ�EV5�¼�(����ģʽ��ѡ��) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* ���ʹ��豸��ַ(дģʽ) */
    I2C_Send7bitAddress(SENSORS_I2Cx, MPU_ADDR, I2C_Direction_Transmitter);

    /* �ȴ�EV6�¼�(��������ģʽ��ѡ��) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* ����Ҫ��ȡ�ļĴ�����ַ */
    I2C_SendData(SENSORS_I2Cx, ReadAddr);

    /* �ȴ�EV8�¼�(��ַ�ֽ��ѷ���) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* �����ظ�START���� */
    I2C_GenerateSTART(SENSORS_I2Cx, ENABLE);

    /* �ȴ�EV5�¼�(����ģʽ��ѡ��) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* ���ʹ��豸��ַ(��ģʽ) */
    I2C_Send7bitAddress(SENSORS_I2Cx, MPU_ADDR, I2C_Direction_Receiver);

    /* �ȴ�EV6�¼�(��������ģʽ��ѡ��) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* ��ȡ���� */
    while (NumByteToRead)
    {
        if (NumByteToRead == 1) // ���һ���ֽ�
        {
            /* ����ACK��Ӧ */
            I2C_AcknowledgeConfig(SENSORS_I2Cx, DISABLE);
            /* ����STOP���� */
            I2C_GenerateSTOP(SENSORS_I2Cx, ENABLE);
        }

        /* �ȴ�EV7�¼�(���յ�����) */
        timeout = I2CT_LONG_TIMEOUT;
        while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            if ((timeout--) == 0)
                return 0;
        }

        /* ��ȡ���� */
        *pBuffer = I2C_ReceiveData(SENSORS_I2Cx);
        pBuffer++;
        NumByteToRead--;
    }

    /* ����ʹ��ACK��Ӧ */
    I2C_AcknowledgeConfig(SENSORS_I2Cx, ENABLE);

    return 1; // ��ȡ�ɹ�
}

/**
 * @brief  �ȴ����豸�������״̬
 * @note   ͨ�����ϳ���ͨ��ֱ���ɹ����жϴ��豸�Ƿ�׼����
 * @param  ��
 * @retval ��
 */
void I2C_WaitStandbyState(void)
{
    vu16 SR1_Tmp = 0;

    do
    {
        /* ����START���� */
        I2C_GenerateSTART(SENSORS_I2Cx, ENABLE);
        /* ��ȡ״̬�Ĵ��� */
        SR1_Tmp = I2C_ReadRegister(SENSORS_I2Cx, I2C_Register_SR1);
        /* ���ʹ��豸��ַ(дģʽ) */
        I2C_Send7bitAddress(SENSORS_I2Cx, MPU_ADDR, I2C_Direction_Transmitter);
    } while (!(I2C_ReadRegister(SENSORS_I2Cx, I2C_Register_SR1) & 0x0002));

    /* ���AF(Ӧ��ʧ��)��־ */
    I2C_ClearFlag(SENSORS_I2Cx, I2C_FLAG_AF);
    /* ����STOP���� */
    I2C_GenerateSTOP(SENSORS_I2Cx, ENABLE);
}
