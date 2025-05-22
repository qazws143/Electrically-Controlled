#ifndef __ST7735_DRIVER_H
#define __ST7735_DRIVER_H
#include "sys.h"
#include "stdlib.h"

// LCD���������ṹ��
typedef struct
{
    u16 width;   // LCD���
    u16 height;  // LCD�߶�
    u16 id;      // LCD�ͺ�ID
    u8 dir;      // ��Ļ����0-������1-����
    u16 wramcmd; // ��ʼдGRAMָ��
    u16 setxcmd; // ����X����ָ��
    u16 setycmd; // ����Y����ָ��
} _lcd_dev;

// LCD�豸ʵ��
extern _lcd_dev lcddev; // LCD�����ṹ��

#define USE_HORIZONTAL 2 // ��Ļ��ת����0-0�ȣ�1-90�ȣ�2-180�ȣ�3-270��

// LCD����ߴ綨��
#define LCD_W 128 // ��Ļ���
#define LCD_H 160 // ��Ļ�߶�

// TFTLCD������ɫ����
extern u16 POINT_COLOR; // Ĭ�ϻ�ͼ��ɫ
extern u16 BACK_COLOR;  // ������ɫ��Ĭ��Ϊ��ɫ

//-----------------LCD���Ŷ���----------------
#define LCD_GPIO_TYPE_A GPIOA
#define LCD_GPIO_TYPE_B GPIOB
#define LCD_CLK_A RCC_APB2Periph_GPIOA
#define LCD_CLK_B RCC_APB2Periph_GPIOB
#define LCD_LED_PIN GPIO_Pin_2  // ����������� PA2
#define LCD_A0_PIN GPIO_Pin_3   // ����/����ѡ�� PA3
#define LCD_RST_PIN GPIO_Pin_14 // ��λ���� PB14
#define LCD_CS_PIN GPIO_Pin_12  // Ƭѡ���� PB12

#define LED 2
#define LCD_CS 12
#define LCD_A0 3
#define LCD_RST 14

// ������ƣ��˴���ʵ��PWM���⣩
#define LCD_LED PAout(LED) // LCD������� PA2
// ע�⣺ʹ��GPIOֱ�ӿ��ƿ�ʵ��Լ14֡/��ˢ����
// ʹ��Ӳ��SPI��ʵ��Լ28֡/��ˢ����

// GPIO��λ����
#define LCD_CS_SET LCD_GPIO_TYPE_B->BSRR = 1 << LCD_CS   // Ƭѡ��λ
#define LCD_A0_SET LCD_GPIO_TYPE_A->BSRR = 1 << LCD_A0   // ����/������λ
#define LCD_RST_SET LCD_GPIO_TYPE_B->BSRR = 1 << LCD_RST // ��λ��λ

// GPIO�������
#define LCD_CS_CLR LCD_GPIO_TYPE_B->BSRR = 1 << (LCD_CS + 16)   // Ƭѡ����
#define LCD_A0_CLR LCD_GPIO_TYPE_A->BSRR = 1 << (LCD_A0 + 16)   // ����/��������
#define LCD_RST_CLR LCD_GPIO_TYPE_B->BSRR = 1 << (LCD_RST + 16) // ��λ����

// ��ɫ����
#define WHITE 0xFFFF   // ��ɫ
#define BLACK 0x0000   // ��ɫ
#define BLUE 0x001F    // ��ɫ
#define BRED 0XF81F    // ����ɫ
#define GRED 0XFFE0    // �̺�ɫ
#define GBLUE 0X07FF   // ����ɫ
#define RED 0xF800     // ��ɫ
#define MAGENTA 0xF81F // Ʒ��
#define GREEN 0x07E0   // ��ɫ
#define CYAN 0x7FFF    // ��ɫ
#define YELLOW 0xFFE0  // ��ɫ
#define BROWN 0XBC40   // ��ɫ
#define BRRED 0XFC07   // ����ɫ
#define GRAY 0X8430    // ��ɫ
// GUI������ɫ

#define DARKBLUE 0X01CF  // ����ɫ
#define LIGHTBLUE 0X7D7C // ǳ��ɫ
#define GRAYBLUE 0X5458  // ����ɫ
// ��Щ��ɫ��������屳��ɫ

#define LIGHTGREEN 0X841F // ǳ��ɫ
#define LIGHTGRAY 0XEF5B  // ǳ��ɫ(PANEL)
#define LGRAY 0XC618      // ǳ��ɫ(PANEL)��������ɫ

#define LGRAYBLUE 0XA651 // ǳ����ɫ(�м�ɫ)
#define LBBLUE 0X2B12    // ǳ����ɫ(�˵���ѡ����ɫ)

// ��������
void LCD_Init(void);                                           // LCD��ʼ��
void LCD_Clear(u16 Color);                                     // ����
void LCD_SetCursor(u16 Xpos, u16 Ypos);                        // ���ù��λ��
void LCD_DrawPoint(u16 x, u16 y);                              // ����
void LCD_SetWindows(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd); // ���ô���

void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue); // д�Ĵ���
void LCD_WR_DATA(u8 data);                       // д����
void LCD_WriteRAM_Prepare(void);                 // ׼��дGRAM
void Lcd_WriteData_16Bit(u16 Data);              // 16λ����д��
void LCD_direction(u8 direction);                // ������ʾ����

#endif
