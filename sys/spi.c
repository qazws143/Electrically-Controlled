#include "spi.h"

/**
 * @brief SPI写数据
 *
 * @param SPIx x:1,2,3
 * @param Byte 数据
 * @return 接收数据
 */
u8 SPI_WriteByte(SPI_TypeDef *SPIx, u8 Byte)
{
    while ((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET)
        ;            // 等待发送区空
    SPIx->DR = Byte; // 发送一个byte
    while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == RESET)
        ; // 等待接收完一个byte
    return SPIx->DR;
}

/**
 * @brief SPI控制LCD初始化
 * LCD不需要向单片机发送数据
 */
void SPI2_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 配置SPI1管脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15; // SCK，SDA
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // SPI2配置选项
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    // 使能SPI2
    SPI_Cmd(SPI2, ENABLE);
}
