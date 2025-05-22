#include "ST7735_Driver.h"
#include "string.h"
#include "font.h"
#include "sys.h"
#include "gui.h"

/**
 * @brief ��ָ���������һ�����ص�
 *
 * @param x ����
 * @param y ����
 * @param color ������ɫֵ
 */
void GUI_DrawPoint(u16 x, u16 y, u16 color)
{
	LCD_SetCursor(x, y); // ���ù��λ��
	Lcd_WriteData_16Bit(color);
}

/**
 * @brief ����������
 *
 * @param sxsy ������ʼ����(���Ͻ�)
 * @param exey ���ν�������(���½�)
 * @param color �����ɫ
 */
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
	u16 i, j;
	u16 width = ex - sx + 1;		// �õ����Ŀ��
	u16 height = ey - sy + 1;		// �߶�
	LCD_SetWindows(sx, sy, ex, ey); // ������ʾ����
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
			Lcd_WriteData_16Bit(color); // д������
	}
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // �ָ���������Ϊȫ��
}

/**
 * @brief ����ֱ��
 *
 * ��ʼ����
 * @param x1
 * @param y1
 * ��ֹ����
 * @param x2
 * @param y2
 */
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;

	delta_x = x2 - x1; // ������������
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;
	if (delta_x > 0)
		incx = 1; // ���õ�������
	else if (delta_x == 0)
		incx = 0; // ��ֱ��
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // ˮƽ��
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // ѡȡ��������������
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) // �������
	{
		LCD_DrawPoint(uRow, uCol); // ����
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
 * @brief ���ƾ��α߿�(���ľ���)
 *
 * �������Ͻ�
 * @param x1
 * @param y1
 * �������½�
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
 * @brief ����������(ʵ�ľ���)
 *
 * �������Ͻ�����
 * @param x1
 * @param y1
 * �������½�����
 * @param x2
 * @param y2
 * @param POINT_COLOR ������ɫ
 */
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 POINT_COLOR)
{
	LCD_Fill(x1, y1, x2, y2, POINT_COLOR);
}

/**
 * @brief ����Բ�� 8 ���ԳƵ�
 *
 * Բ������
 * @param xc
 * @param yc
 * Բ��ƫ�Ƶ�
 * @param x
 * @param y
 * @param c ��ɫ
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
 * @brief ����Բ�����Բ
 *
 * Բ������
 * @param xc
 * @param yc
 * @param c Բ��ɫ
 * @param r �뾶
 * @param fill  0-���ƿ���Բ 1-����ʵ��Բ
 */
void gui_circle(int xc, int yc, u16 c, int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;

	if (fill)
	{
		// �����䣨��ʵ��Բ��
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
		// �������䣨������Բ��
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
 * @brief ���������α߿�(����������)
 *
 * ����1����
 * @param x0
 * @param y0
 * ����2����
 * @param x1
 * @param y1
 * ����3����
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
 * @brief ���������(ʵ��������)
 *
 * ����1����
 * @param x0
 * @param y0
 * ����2����
 * @param x1
 * @param y1
 * ����3����
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
 * @brief ��ʾ����ASCII�ַ�
 *
 * ��ʾλ������
 * @param x
 * @param y
 * @param fc ������ɫ
 * @param bc ������ɫ
 * @param num Ҫ��ʾ���ַ�(ASCII��)
 * @param size �����С(12/16/24/32��)
 * @param mode 0-�ǵ���ģʽ 1-����ģʽ
 */
void LCD_ShowChar(u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 size, u8 mode)
{
	u8 temp;
	u8 pos, t;
	u16 colortemp = POINT_COLOR;

	num = num - ' ';									  // �õ�ƫ�ƺ��ֵ
	LCD_SetWindows(x, y, x + size / 2 - 1, y + size - 1); // ���õ���������ʾ����
	if (!mode)											  // �ǵ��ӷ�ʽ
	{
		for (pos = 0; pos < size; pos++)
		{
			if (size == 12)
				temp = asc2_1206[num][pos]; // ����1206����
			else
				temp = asc2_1608[num][pos]; // ����1608����
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
	else // ���ӷ�ʽ
	{
		for (pos = 0; pos < size; pos++)
		{
			if (size == 12)
				temp = asc2_1206[num][pos]; // ����1206����
			else
				temp = asc2_1608[num][pos]; // ����1608����
			for (t = 0; t < size / 2; t++)
			{
				POINT_COLOR = fc;
				if (temp & 0x01)
					LCD_DrawPoint(x + t, y + pos); // ��һ����
				temp >>= 1;
			}
		}
	}
	POINT_COLOR = colortemp;
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // �ָ�����Ϊȫ��
}

/**
 * @brief ��ʾ�ַ���
 *
 * ��ʾ��ʼ����
 * @param x
 * @param y
 * @param size �����С
 * @param p �ַ���ָ��
 * @param mode 0-�ǵ���ģʽ 1-����ģʽ
 */
void LCD_ShowString(u16 x, u16 y, u8 size, u8 *p, u8 mode)
{
	while ((*p <= '~') && (*p >= ' ')) // �ж��ǲ��ǷǷ��ַ�!
	{
		if (x > (lcddev.width - 1) || y > (lcddev.height - 1))
			return;
		LCD_ShowChar(x, y, POINT_COLOR, BACK_COLOR, *p, size, mode);
		x += size / 2;
		p++;
	}
}

/**
 * @brief ���������ݴη�
 *
 * @param m ����
 * @param n ָ��
 * @return ���ս��
 */
u32 mypow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

/**
 * @brief ��ʾ����
 *
 * ��ʾλ������
 * @param x
 * @param y
 * @param num Ҫ��ʾ������ֵ
 * @param len ��ʾ���ֵ�λ��
 * @param size �����С
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
 * @brief 16����������ʾ
 *
 * ��ʾλ��
 * @param x
 * @param y
 * @param fc ������ɫ
 * @param bc ������ɫ
 * @param s �ַ���ָ��
 * @param mode 0-�ǵ���ģʽ 1-����ģʽ
 */
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode)
{
	u8 i, j;
	u16 k;
	u16 HZnum;
	u16 x0 = x;
	HZnum = sizeof(tfont16) / sizeof(typFNT_GB16); // �Զ�ͳ�ƺ�����Ŀ

	for (k = 0; k < HZnum; k++)
	{
		if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)))
		{
			LCD_SetWindows(x, y, x + 16 - 1, y + 16 - 1);
			for (i = 0; i < 16 * 2; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // �ǵ��ӷ�ʽ
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
							LCD_DrawPoint(x, y); // ��һ����
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
		continue; // ���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}

	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // �ָ�����Ϊȫ��
}

/**
 * @brief 24����������ʾ
 *
 * ��ʾλ��
 * @param x
 * @param y
 * @param fc ������ɫ
 * @param bc ������ɫ
 * @param s �ַ���ָ��
 * @param mode 0-�ǵ���ģʽ 1-����ģʽ
 */
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode)
{
	u8 i, j;
	u16 k;
	u16 HZnum;
	u16 x0 = x;
	HZnum = sizeof(tfont24) / sizeof(typFNT_GB24); // �Զ�ͳ�ƺ�����Ŀ

	for (k = 0; k < HZnum; k++)
	{
		if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)))
		{
			LCD_SetWindows(x, y, x + 24 - 1, y + 24 - 1);
			for (i = 0; i < 24 * 3; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // �ǵ��ӷ�ʽ
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
							LCD_DrawPoint(x, y); // ��һ����
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
		continue; // ���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}

	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // �ָ�����Ϊȫ��
}

/**
 * @brief 32����������ʾ
 *
 * ��ʾλ��
 * @param x
 * @param y
 * @param fc ������ɫ
 * @param bc ������ɫ
 * @param s �ַ���ָ��
 * @param mode 0-�ǵ���ģʽ 1-����ģʽ
 */
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode)
{
	u8 i, j;
	u16 k;
	u16 HZnum;
	u16 x0 = x;
	HZnum = sizeof(tfont32) / sizeof(typFNT_GB32); // �Զ�ͳ�ƺ�����Ŀ
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)))
		{
			LCD_SetWindows(x, y, x + 32 - 1, y + 32 - 1);
			for (i = 0; i < 32 * 4; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // �ǵ��ӷ�ʽ
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
							LCD_DrawPoint(x, y); // ��һ����
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
		continue; // ���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}

	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // �ָ�����Ϊȫ��
}

/**
 * @brief ��ʾ�ַ���
 *
 * ��ʾλ��
 * @param x
 * @param y
 * @param fc ������ɫ
 * @param bc ������ɫ
 * @param str �ַ���
 * @param size �ַ�����С
 * @param mode 0-�ǵ���ģʽ 1-����ģʽ
 */
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str, u8 size, u8 mode)
{
	u16 x0 = x;
	u8 bHz = 0;		  // �ַ���������
	while (*str != 0) // ����δ����
	{
		if (!bHz)
		{
			if (x > (lcddev.width - size / 2) || y > (lcddev.height - size))
				return;
			if (*str > 0x80)
				bHz = 1; // ����
			else		 // �ַ�
			{
				if (*str == 0x0D) // ���з���
				{
					y += size;
					x = x0;
					str++;
				}
				else
				{
					if (size > 16) // �ֿ���û�м���12X24 16X32��Ӣ������,��8X16����
					{
						LCD_ShowChar(x, y, fc, bc, *str, 16, mode);
						x += 8; // �ַ�,Ϊȫ�ֵ�һ��
					}
					else
					{
						LCD_ShowChar(x, y, fc, bc, *str, size, mode);
						x += size / 2; // �ַ�,Ϊȫ�ֵ�һ��
					}
				}
				str++;
			}
		}
		else // ����
		{
			if (x > (lcddev.width - size) || y > (lcddev.height - size))
				return;
			bHz = 0; // �к��ֿ�
			if (size == 32)
				GUI_DrawFont32(x, y, fc, bc, str, mode);
			else if (size == 24)
				GUI_DrawFont24(x, y, fc, bc, str, mode);
			else
				GUI_DrawFont16(x, y, fc, bc, str, mode);

			str += 2;
			x += size; // ��һ������ƫ��
		}
	}
}

/**
 * @brief ������ʾ�ַ���
 *
 * ��ʾ������������
 * @param x
 * @param y
 * @param fc ������ɫ
 * @param bc ������ɫ
 * @param str �ַ���
 * @param size �ַ�����С
 * @param mode 0-�ǵ���ģʽ 1-����ģʽ
 */
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str, u8 size, u8 mode)
{
	u16 len = strlen((const char *)str);
	u16 x1 = (lcddev.width - len * 8) / 2;
	Show_Str(x1, y, fc, bc, str, size, mode);
}

/**
 * @brief ��ʾ16λɫλͼ(40x40����QQͼƬ)
 *
 * ��ʾλ�����Ͻ�����
 * @param x
 * @param y
 * @param p λͼ����ָ��
 */
void Gui_Drawbmp16(u16 x, u16 y, const unsigned char *p) // ��ʾ40*40 QQͼƬ
{
	int i;
	unsigned char picH, picL;
	LCD_SetWindows(x, y, x + 40 - 1, y + 40 - 1); // ��������
	for (i = 0; i < 40 * 40; i++)
	{
		picL = *(p + i * 2); // ���ݵ�λ��ǰ
		picH = *(p + i * 2 + 1);
		Lcd_WriteData_16Bit(picH << 8 | picL);
	}
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // �ָ���ʾ����Ϊȫ��
}
