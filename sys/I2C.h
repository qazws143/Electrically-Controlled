#include "stm32f10x.h"
#include "bsp_usart.h"

#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#define SENSORS_I2Cx I2C1
#define SENSORS_I2C_APBxClock_FUN RCC_APB1PeriphClockCmd
#define SENSORS_I2C_CLK RCC_APB1Periph_I2C1
#define SENSORS_I2C_GPIO_APBxClock_FUN RCC_APB2PeriphClockCmd
#define SENSORS_I2C_GPIO_CLK RCC_APB2Periph_GPIOB
#define SENSORS_I2C_SCL_PORT GPIOB
#define SENSORS_I2C_SCL_PIN GPIO_Pin_6
#define SENSORS_I2C_SDA_PORT GPIOB
#define SENSORS_I2C_SDA_PIN GPIO_Pin_7

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

void I2C_Config(void);
uint8_t I2C_ByteWrite(u8 pBuffer, u8 WriteAddr);
uint8_t I2C_BufferRead(u8 *pBuffer, u8 ReadAddr, u16 NumByteToRead);
void I2C_WaitStandbyState(void);

#endif
