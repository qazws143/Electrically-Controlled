#ifndef __ST7735_DRIVER_H
#define __ST7735_DRIVER_H
#include "sys.h"
#include "stdlib.h"

// LCD基本参数结构体
typedef struct
{
    u16 width;   // LCD宽度
    u16 height;  // LCD高度
    u16 id;      // LCD型号ID
    u8 dir;      // 屏幕方向：0-竖屏，1-横屏
    u16 wramcmd; // 开始写GRAM指令
    u16 setxcmd; // 设置X坐标指令
    u16 setycmd; // 设置Y坐标指令
} _lcd_dev;

// LCD设备实例
extern _lcd_dev lcddev; // LCD参数结构体

#define USE_HORIZONTAL 2 // 屏幕旋转方向：0-0度，1-90度，2-180度，3-270度

// LCD物理尺寸定义
#define LCD_W 128 // 屏幕宽度
#define LCD_H 160 // 屏幕高度

// TFTLCD常用颜色定义
extern u16 POINT_COLOR; // 默认绘图颜色
extern u16 BACK_COLOR;  // 背景颜色，默认为白色

//-----------------LCD引脚定义----------------
#define LCD_GPIO_TYPE_A GPIOA
#define LCD_GPIO_TYPE_B GPIOB
#define LCD_CLK_A RCC_APB2Periph_GPIOA
#define LCD_CLK_B RCC_APB2Periph_GPIOB
#define LCD_LED_PIN GPIO_Pin_2  // 背光控制引脚 PA2
#define LCD_A0_PIN GPIO_Pin_3   // 数据/命令选择 PA3
#define LCD_RST_PIN GPIO_Pin_14 // 复位引脚 PB14
#define LCD_CS_PIN GPIO_Pin_12  // 片选引脚 PB12

#define LED 2
#define LCD_CS 12
#define LCD_A0 3
#define LCD_RST 14

// 背光控制（此处可实现PWM调光）
#define LCD_LED PAout(LED) // LCD背光控制 PA2
// 注意：使用GPIO直接控制可实现约14帧/秒刷新率
// 使用硬件SPI可实现约28帧/秒刷新率

// GPIO置位操作
#define LCD_CS_SET LCD_GPIO_TYPE_B->BSRR = 1 << LCD_CS   // 片选置位
#define LCD_A0_SET LCD_GPIO_TYPE_A->BSRR = 1 << LCD_A0   // 数据/命令置位
#define LCD_RST_SET LCD_GPIO_TYPE_B->BSRR = 1 << LCD_RST // 复位置位

// GPIO清零操作
#define LCD_CS_CLR LCD_GPIO_TYPE_B->BSRR = 1 << (LCD_CS + 16)   // 片选清零
#define LCD_A0_CLR LCD_GPIO_TYPE_A->BSRR = 1 << (LCD_A0 + 16)   // 数据/命令清零
#define LCD_RST_CLR LCD_GPIO_TYPE_B->BSRR = 1 << (LCD_RST + 16) // 复位清零

// 颜色定义
#define WHITE 0xFFFF   // 白色
#define BLACK 0x0000   // 黑色
#define BLUE 0x001F    // 蓝色
#define BRED 0XF81F    // 蓝红色
#define GRED 0XFFE0    // 绿红色
#define GBLUE 0X07FF   // 蓝绿色
#define RED 0xF800     // 红色
#define MAGENTA 0xF81F // 品红
#define GREEN 0x07E0   // 绿色
#define CYAN 0x7FFF    // 青色
#define YELLOW 0xFFE0  // 黄色
#define BROWN 0XBC40   // 棕色
#define BRRED 0XFC07   // 亮红色
#define GRAY 0X8430    // 灰色
// GUI常用颜色

#define DARKBLUE 0X01CF  // 深蓝色
#define LIGHTBLUE 0X7D7C // 浅蓝色
#define GRAYBLUE 0X5458  // 灰蓝色
// 这些颜色常用作面板背景色

#define LIGHTGREEN 0X841F // 浅绿色
#define LIGHTGRAY 0XEF5B  // 浅灰色(PANEL)
#define LGRAY 0XC618      // 浅灰色(PANEL)，表单背景色

#define LGRAYBLUE 0XA651 // 浅灰蓝色(中间色)
#define LBBLUE 0X2B12    // 浅蓝蓝色(菜单项选中颜色)

// 函数声明
void LCD_Init(void);                                           // LCD初始化
void LCD_Clear(u16 Color);                                     // 清屏
void LCD_SetCursor(u16 Xpos, u16 Ypos);                        // 设置光标位置
void LCD_DrawPoint(u16 x, u16 y);                              // 画点
void LCD_SetWindows(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd); // 设置窗口

void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue); // 写寄存器
void LCD_WR_DATA(u8 data);                       // 写数据
void LCD_WriteRAM_Prepare(void);                 // 准备写GRAM
void Lcd_WriteData_16Bit(u16 Data);              // 16位数据写入
void LCD_direction(u8 direction);                // 设置显示方向

#endif
