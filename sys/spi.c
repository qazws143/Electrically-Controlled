#include "spi.h"

/**
 * @brief SPIд����
 *
 * @param SPIx x:1,2,3
 * @param Byte ����
 * @return ��������
 */
u8 SPI_WriteByte(SPI_TypeDef *SPIx, u8 Byte)
{
    while ((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET)
        ;            // �ȴ���������
    SPIx->DR = Byte; // ����һ��byte
    while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == RESET)
        ; // �ȴ�������һ��byte
    return SPIx->DR;
}

/**
 * @brief SPI����LCD��ʼ��
 * LCD����Ҫ��Ƭ����������
 */
void SPI2_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // ����SPI1�ܽ�
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15; // SCK��SDA
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // SPI2����ѡ��
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

    // ʹ��SPI2
    SPI_Cmd(SPI2, ENABLE);
}
