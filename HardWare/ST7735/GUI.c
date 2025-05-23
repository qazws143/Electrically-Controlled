#include "ST7735_Driver.h"
#include "string.h"
#include "font.h"
#include "sys.h"
#include "gui.h"

/**
 * @brief 在指定坐标绘制一个像素点
 *
 * @param x 横轴
 * @param y 纵轴
 * @param color 像素颜色值
 */
void GUI_DrawPoint(u16 x, u16 y, u16 color)
{
	LCD_SetCursor(x, y); // 设置光标位置
	Lcd_WriteData_16Bit(color);
}

/**
 * @brief 填充矩形区域
 *
 * @param sxsy 矩形起始坐标(左上角)
 * @param exey 矩形结束坐标(右下角)
 * @param color 填充颜色
 */
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
	u16 i, j;
	u16 width = ex - sx + 1;		// 得到填充的宽度
	u16 height = ey - sy + 1;		// 高度
	LCD_SetWindows(sx, sy, ex, ey); // 设置显示窗口
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
			Lcd_WriteData_16Bit(color); // 写入数据
	}
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // 恢复窗口设置为全屏
}

/**
 * @brief 绘制直线
 *
 * 起始坐标
 * @param x1
 * @param y1
 * 终止坐标
 * @param x2
 * @param y2
 */
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;

	delta_x = x2 - x1; // 计算坐标增量
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;
	if (delta_x > 0)
		incx = 1; // 设置单步方向
	else if (delta_x == 0)
		incx = 0; // 垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // 水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // 选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) // 画线输出
	{
		LCD_DrawPoint(uRow, uCol); // 画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

/**
 * @brief 绘制矩形边框(空心矩形)
 *
 * 矩形左上角
 * @param x1
 * @param y1
 * 矩形右下角
 * @param x2
 * @param y2
 */
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1, y1, x2, y1);
	LCD_DrawLine(x1, y1, x1, y2);
	LCD_DrawLine(x1, y2, x2, y2);
	LCD_DrawLine(x2, y1, x2, y2);
}

/**
 * @brief 绘制填充矩形(实心矩形)
 *
 * 矩形左上角坐标
 * @param x1
 * @param y1
 * 矩形右下角坐标
 * @param x2
 * @param y2
 * @param POINT_COLOR 矩形颜色
 */
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 POINT_COLOR)
{
	LCD_Fill(x1, y1, x2, y2, POINT_COLOR);
}

/**
 * @brief 绘制圆的 8 个对称点
 *
 * 圆心坐标
 * @param xc
 * @param yc
 * 圆心偏移点
 * @param x
 * @param y
 * @param c 颜色
 */
void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
{
	GUI_DrawPoint(xc + x, yc + y, c);

	GUI_DrawPoint(xc - x, yc + y, c);

	GUI_DrawPoint(xc + x, yc - y, c);

	GUI_DrawPoint(xc - x, yc - y, c);

	GUI_DrawPoint(xc + y, yc + x, c);

	GUI_DrawPoint(xc - y, yc + x, c);

	GUI_DrawPoint(xc + y, yc - x, c);

	GUI_DrawPoint(xc - y, yc - x, c);
}

/**
 * @brief 绘制圆或填充圆
 *
 * 圆心坐标
 * @param xc
 * @param yc
 * @param c 圆颜色
 * @param r 半径
 * @param fill  0-绘制空心圆 1-绘制实心圆
 */
void gui_circle(int xc, int yc, u16 c, int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;

	if (fill)
	{
		// 如果填充（画实心圆）
		while (x <= y)
		{
			for (yi = x; yi <= y; yi++)
				_draw_circle_8(xc, yc, x, yi, c);

			if (d < 0)
			{
				d = d + 4 * x + 6;
			}
			else
			{
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
	else
	{
		// 如果不填充（画空心圆）
		while (x <= y)
		{
			_draw_circle_8(xc, yc, x, y, c);
			if (d < 0)
			{
				d = d + 4 * x + 6;
			}
			else
			{
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}

/**
 * @brief 绘制三角形边框(空心三角形)
 *
 * 顶点1坐标
 * @param x0
 * @param y0
 * 顶点2坐标
 * @param x1
 * @param y1
 * 顶点3坐标
 * @param x2
 * @param y2
 */
void Draw_Triangel(u16 x0, u16 y0, u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x0, y0, x1, y1);
	LCD_DrawLine(x1, y1, x2, y2);
	LCD_DrawLine(x2, y2, x0, y0);
}

static void _swap(u16 *a, u16 *b)
{
	u16 tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

/**
 * @brief 填充三角形(实心三角形)
 *
 * 顶点1坐标
 * @param x0
 * @param y0
 * 顶点2坐标
 * @param x1
 * @param y1
 * 顶点3坐标
 * @param x2
 * @param y2
 */
void Fill_Triangel(u16 x0, u16 y0, u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 a, b, y, last;
	int dx01, dy01, dx02, dy02, dx12, dy12;
	long sa = 0;
	long sb = 0;
	if (y0 > y1)
	{
		_swap(&y0, &y1);
		_swap(&x0, &x1);
	}
	if (y1 > y2)
	{
		_swap(&y2, &y1);
		_swap(&x2, &x1);
	}
	if (y0 > y1)
	{
		_swap(&y0, &y1);
		_swap(&x0, &x1);
	}
	if (y0 == y2)
	{
		a = b = x0;
		if (x1 < a)
		{
			a = x1;
		}
		else if (x1 > b)
		{
			b = x1;
		}
		if (x2 < a)
		{
			a = x2;
		}
		else if (x2 > b)
		{
			b = x2;
		}
		LCD_Fill(a, y0, b, y0, POINT_COLOR);
		return;
	}
	dx01 = x1 - x0;
	dy01 = y1 - y0;
	dx02 = x2 - x0;
	dy02 = y2 - y0;
	dx12 = x2 - x1;
	dy12 = y2 - y1;

	if (y1 == y2)
	{
		last = y1;
	}
	else
	{
		last = y1 - 1;
	}
	for (y = y0; y <= last; y++)
	{
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		if (a > b)
		{
			_swap(&a, &b);
		}
		LCD_Fill(a, y, b, y, POINT_COLOR);
	}
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++)
	{
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if (a > b)
		{
			_swap(&a, &b);
		}
		LCD_Fill(a, y, b, y, POINT_COLOR);
	}
}

/**
 * @brief 显示单个ASCII字符
 *
 * 显示位置坐标
 * @param x
 * @param y
 * @param fc 字体颜色
 * @param bc 背景颜色
 * @param num 要显示的字符(ASCII码)
 * @param size 字体大小(12/16/24/32等)
 * @param mode 0-非叠加模式 1-叠加模式
 */
void LCD_ShowChar(u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 size, u8 mode)
{
	u8 temp;
	u8 pos, t;
	u16 colortemp = POINT_COLOR;

	num = num - ' ';									  // 得到偏移后的值
	LCD_SetWindows(x, y, x + size / 2 - 1, y + size - 1); // 设置单个文字显示窗口
	if (!mode)											  // 非叠加方式
	{
		for (pos = 0; pos < size; pos++)
		{
			if (size == 12)
				temp = asc2_1206[num][pos]; // 调用1206字体
			else
				temp = asc2_1608[num][pos]; // 调用1608字体
			for (t = 0; t < size / 2; t++)
			{
				if (temp & 0x01)
					Lcd_WriteData_16Bit(fc);
				else
					Lcd_WriteData_16Bit(bc);
				temp >>= 1;
			}
		}
	}
	else // 叠加方式
	{
		for (pos = 0; pos < size; pos++)
		{
			if (size == 12)
				temp = asc2_1206[num][pos]; // 调用1206字体
			else
				temp = asc2_1608[num][pos]; // 调用1608字体
			for (t = 0; t < size / 2; t++)
			{
				POINT_COLOR = fc;
				if (temp & 0x01)
					LCD_DrawPoint(x + t, y + pos); // 画一个点
				temp >>= 1;
			}
		}
	}
	POINT_COLOR = colortemp;
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // 恢复窗口为全屏
}

/**
 * @brief 显示字符串
 *
 * 显示起始坐标
 * @param x
 * @param y
 * @param size 字体大小
 * @param p 字符串指针
 * @param mode 0-非叠加模式 1-叠加模式
 */
void LCD_ShowString(u16 x, u16 y, u8 size, u8 *p, u8 mode)
{
	while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
	{
		if (x > (lcddev.width - 1) || y > (lcddev.height - 1))
			return;
		LCD_ShowChar(x, y, POINT_COLOR, BACK_COLOR, *p, size, mode);
		x += size / 2;
		p++;
	}
}

/**
 * @brief 计算整数幂次方
 *
 * @param m 底数
 * @param n 指数
 * @return 最终结果
 */
u32 mypow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

/**
 * @brief 显示数字
 *
 * 显示位置坐标
 * @param x
 * @param y
 * @param num 要显示的数字值
 * @param len 显示数字的位数
 * @param size 字体大小
 */
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				LCD_ShowChar(x + (size / 2) * t, y, POINT_COLOR, BACK_COLOR, ' ', size, 0);
				continue;
			}
			else
				enshow = 1;
		}
		LCD_ShowChar(x + (size / 2) * t, y, POINT_COLOR, BACK_COLOR, temp + '0', size, 0);
	}
}

/**
 * @brief 16点阵字体显示
 *
 * 显示位置
 * @param x
 * @param y
 * @param fc 字体颜色
 * @param bc 背景颜色
 * @param s 字符串指针
 * @param mode 0-非叠加模式 1-叠加模式
 */
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode)
{
	u8 i, j;
	u16 k;
	u16 HZnum;
	u16 x0 = x;
	HZnum = sizeof(tfont16) / sizeof(typFNT_GB16); // 自动统计汉字数目

	for (k = 0; k < HZnum; k++)
	{
		if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)))
		{
			LCD_SetWindows(x, y, x + 16 - 1, y + 16 - 1);
			for (i = 0; i < 16 * 2; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont16[k].Msk[i] & (0x80 >> j))
							Lcd_WriteData_16Bit(fc);
						else
							Lcd_WriteData_16Bit(bc);
					}
					else
					{
						POINT_COLOR = fc;
						if (tfont16[k].Msk[i] & (0x80 >> j))
							LCD_DrawPoint(x, y); // 画一个点
						x++;
						if ((x - x0) == 16)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // 恢复窗口为全屏
}

/**
 * @brief 24点阵字体显示
 *
 * 显示位置
 * @param x
 * @param y
 * @param fc 字体颜色
 * @param bc 背景颜色
 * @param s 字符串指针
 * @param mode 0-非叠加模式 1-叠加模式
 */
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode)
{
	u8 i, j;
	u16 k;
	u16 HZnum;
	u16 x0 = x;
	HZnum = sizeof(tfont24) / sizeof(typFNT_GB24); // 自动统计汉字数目

	for (k = 0; k < HZnum; k++)
	{
		if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)))
		{
			LCD_SetWindows(x, y, x + 24 - 1, y + 24 - 1);
			for (i = 0; i < 24 * 3; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont24[k].Msk[i] & (0x80 >> j))
							Lcd_WriteData_16Bit(fc);
						else
							Lcd_WriteData_16Bit(bc);
					}
					else
					{
						POINT_COLOR = fc;
						if (tfont24[k].Msk[i] & (0x80 >> j))
							LCD_DrawPoint(x, y); // 画一个点
						x++;
						if ((x - x0) == 24)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // 恢复窗口为全屏
}

/**
 * @brief 32点阵字体显示
 *
 * 显示位置
 * @param x
 * @param y
 * @param fc 字体颜色
 * @param bc 背景颜色
 * @param s 字符串指针
 * @param mode 0-非叠加模式 1-叠加模式
 */
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode)
{
	u8 i, j;
	u16 k;
	u16 HZnum;
	u16 x0 = x;
	HZnum = sizeof(tfont32) / sizeof(typFNT_GB32); // 自动统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)))
		{
			LCD_SetWindows(x, y, x + 32 - 1, y + 32 - 1);
			for (i = 0; i < 32 * 4; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont32[k].Msk[i] & (0x80 >> j))
							Lcd_WriteData_16Bit(fc);
						else
							Lcd_WriteData_16Bit(bc);
					}
					else
					{
						POINT_COLOR = fc;
						if (tfont32[k].Msk[i] & (0x80 >> j))
							LCD_DrawPoint(x, y); // 画一个点
						x++;
						if ((x - x0) == 32)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // 恢复窗口为全屏
}

/**
 * @brief 显示字符串
 *
 * 显示位置
 * @param x
 * @param y
 * @param fc 字体颜色
 * @param bc 背景颜色
 * @param str 字符串
 * @param size 字符串大小
 * @param mode 0-非叠加模式 1-叠加模式
 */
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str, u8 size, u8 mode)
{
	u16 x0 = x;
	u8 bHz = 0;		  // 字符或者中文
	while (*str != 0) // 数据未结束
	{
		if (!bHz)
		{
			if (x > (lcddev.width - size / 2) || y > (lcddev.height - size))
				return;
			if (*str > 0x80)
				bHz = 1; // 中文
			else		 // 字符
			{
				if (*str == 0x0D) // 换行符号
				{
					y += size;
					x = x0;
					str++;
				}
				else
				{
					if (size > 16) // 字库中没有集成12X24 16X32的英文字体,用8X16代替
					{
						LCD_ShowChar(x, y, fc, bc, *str, 16, mode);
						x += 8; // 字符,为全字的一半
					}
					else
					{
						LCD_ShowChar(x, y, fc, bc, *str, size, mode);
						x += size / 2; // 字符,为全字的一半
					}
				}
				str++;
			}
		}
		else // 中文
		{
			if (x > (lcddev.width - size) || y > (lcddev.height - size))
				return;
			bHz = 0; // 有汉字库
			if (size == 32)
				GUI_DrawFont32(x, y, fc, bc, str, mode);
			else if (size == 24)
				GUI_DrawFont24(x, y, fc, bc, str, mode);
			else
				GUI_DrawFont16(x, y, fc, bc, str, mode);

			str += 2;
			x += size; // 下一个汉字偏移
		}
	}
}

/**
 * @brief 居中显示字符串
 *
 * 显示区域中心坐标
 * @param x
 * @param y
 * @param fc 字体颜色
 * @param bc 背景颜色
 * @param str 字符串
 * @param size 字符串大小
 * @param mode 0-非叠加模式 1-叠加模式
 */
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str, u8 size, u8 mode)
{
	u16 len = strlen((const char *)str);
	u16 x1 = (lcddev.width - len * 8) / 2;
	Show_Str(x1, y, fc, bc, str, size, mode);
}

/**
 * @brief 显示16位色位图(40x40像素QQ图片)
 *
 * 显示位置左上角坐标
 * @param x
 * @param y
 * @param p 位图数据指针
 */
void Gui_Drawbmp16(u16 x, u16 y, const unsigned char *p) // 显示40*40 QQ图片
{
	int i;
	unsigned char picH, picL;
	LCD_SetWindows(x, y, x + 40 - 1, y + 40 - 1); // 窗口设置
	for (i = 0; i < 40 * 40; i++)
	{
		picL = *(p + i * 2); // 数据低位在前
		picH = *(p + i * 2 + 1);
		Lcd_WriteData_16Bit(picH << 8 | picL);
	}
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // 恢复显示窗口为全屏
}
