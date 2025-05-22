#include "I2C.h"
#include "../HardWare/mpu6050/mpu6050.h"

/* 全局变量定义 */
static __IO uint32_t I2CTimeout = I2CT_LONG_TIMEOUT; // I2C操作超时计数器

// 本函数内需要加入等待时间，否则容易读取失败

/**
 * @brief  I2C初始化
 * @note   配置I2C为400kHz标准模式，7位地址模式
 * @param  无
 * @retval 无
 */
void I2C_Config(void)
{
    I2C_InitTypeDef I2C_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能I2C外设时钟和GPIO时钟 */
    SENSORS_I2C_APBxClock_FUN(SENSORS_I2C_CLK, ENABLE);
    SENSORS_I2C_GPIO_APBxClock_FUN(SENSORS_I2C_GPIO_CLK, ENABLE);

    /* 配置SCL引脚(PB6) */
    GPIO_InitStructure.GPIO_Pin = SENSORS_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 高速模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;   // 复用开漏输出
    GPIO_Init(SENSORS_I2C_SCL_PORT, &GPIO_InitStructure);

    /* 配置SDA引脚(PB7) */
    GPIO_InitStructure.GPIO_Pin = SENSORS_I2C_SDA_PIN;
    GPIO_Init(SENSORS_I2C_SDA_PORT, &GPIO_InitStructure);

    /* I2C 基本参数配置 */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                // I2C模式
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                        // Tlow/Thigh = 2
    I2C_InitStructure.I2C_OwnAddress1 = 0xff;                                 // 主机地址(未使用)
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                               // 使能ACK响应
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7位地址模式
    I2C_InitStructure.I2C_ClockSpeed = 400000;                                // 400kHz标准模式

    /*DMA读取数据*/

    /* 初始化I2C */
    I2C_Init(SENSORS_I2Cx, &I2C_InitStructure);

    /* 使能I2C */
    I2C_Cmd(SENSORS_I2Cx, ENABLE);
}

/**
 * @brief  向I2C设备写入一个字节数据
 * @param  pBuffer: 要写入的数据
 * @param  WriteAddr: 从设备内部寄存器地址
 * @retval 1: 写入成功 0: 写入失败(超时)
 */
uint8_t I2C_ByteWrite(u8 pBuffer, u8 WriteAddr)
{
    uint32_t timeout = I2CT_LONG_TIMEOUT;
    I2C_GenerateSTART(SENSORS_I2Cx, ENABLE);

    /* 等待EV5事件(主机模式已选择) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* 发送从设备地址(写模式) */
    I2C_Send7bitAddress(SENSORS_I2Cx, MPU_ADDR, I2C_Direction_Transmitter);

    /* 等待EV6事件(主机发送模式已选择) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* 发送要读取的寄存器地址 */
    I2C_SendData(SENSORS_I2Cx, WriteAddr);

    /* 等待EV8事件(地址字节已发送) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((timeout--) == 0)
            return 0;
    }
    /* 发送要写入的数据 */
    I2C_SendData(SENSORS_I2Cx, pBuffer);

    /* 等待EV8事件(数据字节已发送) */
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((timeout--) == 0)
            return 0;
    }
    /* 发送STOP条件 */
    I2C_GenerateSTOP(SENSORS_I2Cx, ENABLE);

    return 1; // 写入成功
}

/**
 * @brief  从I2C设备读取多个字节数据
 * @param  pBuffer: 数据存储缓冲区指针
 * @param  ReadAddr: 从设备内部起始寄存器地址
 * @param  NumByteToRead: 要读取的字节数
 * @retval 1: 读取成功 0: 读取失败(超时)
 *
 */
uint8_t I2C_BufferRead(u8 *pBuffer, u8 ReadAddr, u16 NumByteToRead)
{
    /* 等待总线空闲 */
    uint32_t timeout = I2CT_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(SENSORS_I2Cx, I2C_FLAG_BUSY))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* 发送START条件 */
    I2C_GenerateSTART(SENSORS_I2Cx, ENABLE);

    /* 等待EV5事件(主机模式已选择) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* 发送从设备地址(写模式) */
    I2C_Send7bitAddress(SENSORS_I2Cx, MPU_ADDR, I2C_Direction_Transmitter);

    /* 等待EV6事件(主机发送模式已选择) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* 发送要读取的寄存器地址 */
    I2C_SendData(SENSORS_I2Cx, ReadAddr);

    /* 等待EV8事件(地址字节已发送) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* 发送重复START条件 */
    I2C_GenerateSTART(SENSORS_I2Cx, ENABLE);

    /* 等待EV5事件(主机模式已选择) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* 发送从设备地址(读模式) */
    I2C_Send7bitAddress(SENSORS_I2Cx, MPU_ADDR, I2C_Direction_Receiver);

    /* 等待EV6事件(主机接收模式已选择) */
    timeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if ((timeout--) == 0)
            return 0;
    }

    /* 读取数据 */
    while (NumByteToRead)
    {
        if (NumByteToRead == 1) // 最后一个字节
        {
            /* 禁用ACK响应 */
            I2C_AcknowledgeConfig(SENSORS_I2Cx, DISABLE);
            /* 发送STOP条件 */
            I2C_GenerateSTOP(SENSORS_I2Cx, ENABLE);
        }

        /* 等待EV7事件(接收到数据) */
        timeout = I2CT_LONG_TIMEOUT;
        while (!I2C_CheckEvent(SENSORS_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            if ((timeout--) == 0)
                return 0;
        }

        /* 读取数据 */
        *pBuffer = I2C_ReceiveData(SENSORS_I2Cx);
        pBuffer++;
        NumByteToRead--;
    }

    /* 重新使能ACK响应 */
    I2C_AcknowledgeConfig(SENSORS_I2Cx, ENABLE);

    return 1; // 读取成功
}

/**
 * @brief  等待从设备进入待机状态
 * @note   通过不断尝试通信直到成功来判断从设备是否准备好
 * @param  无
 * @retval 无
 */
void I2C_WaitStandbyState(void)
{
    vu16 SR1_Tmp = 0;

    do
    {
        /* 发送START条件 */
        I2C_GenerateSTART(SENSORS_I2Cx, ENABLE);
        /* 读取状态寄存器 */
        SR1_Tmp = I2C_ReadRegister(SENSORS_I2Cx, I2C_Register_SR1);
        /* 发送从设备地址(写模式) */
        I2C_Send7bitAddress(SENSORS_I2Cx, MPU_ADDR, I2C_Direction_Transmitter);
    } while (!(I2C_ReadRegister(SENSORS_I2Cx, I2C_Register_SR1) & 0x0002));

    /* 清除AF(应答失败)标志 */
    I2C_ClearFlag(SENSORS_I2Cx, I2C_FLAG_AF);
    /* 发送STOP条件 */
    I2C_GenerateSTOP(SENSORS_I2Cx, ENABLE);
}
