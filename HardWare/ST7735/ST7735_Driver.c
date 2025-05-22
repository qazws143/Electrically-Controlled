//       SDA--PB15         //液晶屏SPI总线数据写信号
//       LED--PA2         //液晶屏背光控制信号，如果不需要控制，接5V或3.3V
//       SCK--PB13         //液晶屏SPI总线时钟信号
//       A0--PA3         //液晶屏数据/命令控制信号
//       RESET--PB14         //液晶屏复位控制信号
//       CS--PB12       //液晶屏片选控制信号
#include "ST7735_Driver.h"
#include "spi.h"
#include "delay.h"

// 默认为竖屏
_lcd_dev lcddev;

// 画笔颜色,背景颜色
u16 POINT_COLOR = 0x0000, BACK_COLOR = 0xFFFF;
u16 DeviceCode;

/**
 * @brief LCD引脚初始化
 *
 */
void LCD_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(LCD_CLK_A, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LCD_LED_PIN | LCD_A0_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_Init(LCD_GPIO_TYPE_A, &GPIO_InitStructure); // 初始化

	RCC_APB2PeriphClockCmd(LCD_CLK_B, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN | LCD_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_Init(LCD_GPIO_TYPE_B, &GPIO_InitStructure); // 初始化s
}

/**
 * @brief 向LCD屏幕写入8位命令‌
 *
 * @param data 命令
 */
void LCD_WR_REG(u8 data)
{
	LCD_CS_CLR;
	LCD_A0_CLR;
	SPI_WriteByte(SPI2, data);
	LCD_CS_SET;
}

/**
 * @brief 向LCD写8位数据
 *
 * @param data 数据
 */
void LCD_WR_DATA(u8 data)
{
	LCD_CS_CLR;
	LCD_A0_SET;
	SPI_WriteByte(SPI2, data);
	LCD_CS_SET;
}

/**
 * @brief 向LCD的寄存器写入数据
 *
 * @param LCD_Reg LCD寄存器的地址
 * @param LCD_RegValue 要写入寄存器的数据值
 */
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}

/**
 * @brief 将数据写入LCD的显示内存
 *
 */
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}

/**
 * @brief 写入16字节数据
 *
 * @param Data 数据
 */
void Lcd_WriteData_16Bit(u16 Data)
{
	LCD_CS_CLR;
	LCD_A0_SET;
	SPI_WriteByte(SPI2, Data >> 8);
	SPI_WriteByte(SPI2, Data);
	LCD_CS_SET;
}

/**
 * @brief 画点
 *
 * 坐标
 * @param x
 * @param y
 */
void LCD_DrawPoint(u16 x, u16 y)
{
	LCD_SetCursor(x, y); // 设置光标位置
	Lcd_WriteData_16Bit(POINT_COLOR);
}

/**
 * @brief 清屏
 *
 * @param Color 颜色
 */
void LCD_Clear(u16 Color)
{
	unsigned int i, m;
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1);
	LCD_CS_CLR;
	LCD_A0_SET;
	for (i = 0; i < lcddev.height; i++)
	{
		for (m = 0; m < lcddev.width; m++)
		{
			Lcd_WriteData_16Bit(Color);
		}
	}
	LCD_CS_SET;
}

/**
 * @brief ST7735S复位
 *
 */
void LCD_RESET(void)
{
	LCD_RST_CLR;
	Delay_ms(100);
	LCD_RST_SET;
	Delay_ms(50);
}

/**
 * @brief ST7735S初始化
 *
 */
void LCD_Init(void)
{
	SPI2_Init();	// 硬件SPI2初始化
	LCD_GPIOInit(); // LCD GPIO初始化
	LCD_RESET();	// LCD 复位
	//************* ST7735S初始化**********//
	LCD_WR_REG(0x11); // 退出睡眠
	Delay_ms(120);
	// ST7735R 帧率控制
	LCD_WR_REG(0xB1);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x2D);
	LCD_WR_REG(0xB2);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x2D);
	LCD_WR_REG(0xB3);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x2D);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x2D);
	LCD_WR_REG(0xB4); // 极性反转
	LCD_WR_DATA(0x07);
	// ST7735R 上电时序
	LCD_WR_REG(0xC0);
	LCD_WR_DATA(0xA2);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x84);
	LCD_WR_REG(0xC1);
	LCD_WR_DATA(0xC5);
	LCD_WR_REG(0xC2);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xC3);
	LCD_WR_DATA(0x8A);
	LCD_WR_DATA(0x2A);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA(0x8A);
	LCD_WR_DATA(0xEE);
	LCD_WR_REG(0xC5);
	LCD_WR_DATA(0x0E);
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0xC0);
	// ST7735R 伽马校正时序
	LCD_WR_REG(0xe0);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x1a);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x2f);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x20);
	LCD_WR_DATA(0x22);
	LCD_WR_DATA(0x1f);
	LCD_WR_DATA(0x1b);
	LCD_WR_DATA(0x23);
	LCD_WR_DATA(0x37);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x10);
	LCD_WR_REG(0xe1);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x1b);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x2c);
	LCD_WR_DATA(0x29);
	LCD_WR_DATA(0x2e);
	LCD_WR_DATA(0x30);
	LCD_WR_DATA(0x30);
	LCD_WR_DATA(0x39);
	LCD_WR_DATA(0x3f);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x10);
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x7f);
	LCD_WR_REG(0x2b);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x9f);
	LCD_WR_REG(0xF0);
	LCD_WR_DATA(0x01);
	LCD_WR_REG(0xF6);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0x3A);
	LCD_WR_DATA(0x05);
	LCD_WR_REG(0x29);			   // Display on
	LCD_direction(USE_HORIZONTAL); // 设置LCD显示方向
	LCD_LED = 1;				   // 点亮背光
	LCD_Clear(WHITE);			   // 清全屏白色
}

/**
 * @brief 设置LCD显示窗口的显示区域
 *
 * 起始坐标
 * @param xStar
 * @param yStar
 * 终止坐标
 * @param xEnd
 * @param yEnd
 */
void LCD_SetWindows(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd)
{
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(xStar);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(xEnd);

	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(yStar);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(yEnd);

	LCD_WriteRAM_Prepare(); // 开始写入GRAM
}

/**
 * @brief 设置LCD屏幕上的光标位置
 *
 * @param Xpos 横坐标
 * @param Ypos 纵坐标
 */
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	LCD_SetWindows(Xpos, Ypos, Xpos, Ypos);
}

/**
 * @briefs LCD屏幕转向
 *
 * @param direction 旋转角度
 */
void LCD_direction(u8 direction)
{
	lcddev.setxcmd = 0x2A;
	lcddev.setycmd = 0x2B;
	lcddev.wramcmd = 0x2C;
	switch (direction)
	{
	case 0: // 默认方向
		lcddev.width = LCD_W;
		lcddev.height = LCD_H;
		LCD_WriteReg(0x36, (0 << 3) | (1 << 6) | (1 << 7)); // BGR==1,MY==0,MX==0,MV==0
		break;
	case 1: // 旋转 90°
		lcddev.width = LCD_H;
		lcddev.height = LCD_W;
		LCD_WriteReg(0x36, (0 << 3) | (1 << 7) | (1 << 5)); // BGR==1,MY==1,MX==0,MV==1
		break;
	case 2: // 旋转 180°
		lcddev.width = LCD_W;
		lcddev.height = LCD_H;
		LCD_WriteReg(0x36, (0 << 3) | (0 << 6) | (0 << 7)); // BGR==1,MY==0,MX==0,MV==0
		break;
	case 3: // 旋转 270°
		lcddev.width = LCD_H;
		lcddev.height = LCD_W;
		LCD_WriteReg(0x36, (0 << 3) | (0 << 7) | (1 << 6) | (1 << 5)); // BGR==1,MY==1,MX==0,MV==1
		break;
	default:
		break;
	}
}
