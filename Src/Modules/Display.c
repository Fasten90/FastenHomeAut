/*
 *		Display.c
 *
 *		Created on:		2017. ápr. 2.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. ápr. 2.
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "Display.h"

#ifdef CONFIG_MODULE_DISPLAY_ENABLE

#ifdef CONFIG_MODULE_DISPLAY_FONT8X5_ENABLE
#include "Font8x5.h"
#endif
#ifdef CONFIG_MODULE_DISPLAY_FONT12X8_ENABLE
#include "Font12x8.h"
#endif



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static bool CarActualState = false;
bool Display_CarAnimationDisable_flag = false;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void Display_FillRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



void Display_PrintString(const char *str, uint8_t line, Font_Type font)
{

	uint8_t i;

	for (i = 0; str[i]; i++)
	{
		switch (font)
		{
			case Font_8x5:
				Display_PrintFont8x5(str[i], i, line);
				break;

			case Font_12x8:
				Display_PrintFont12x8(str[i], i, line);
				break;

			case Font_Unknown:
			case Font_Count:
			default:
				break;
		}
	}
}



void Display_PrintFont8x5(uint8_t chr, uint8_t index, uint8_t line)
{
	// 8x5 pixel font
	uint8_t i;
	uint8_t j;

	// Step on columns
	for (i = 0; i < FONT_8X5_WIDTH; i++)
	{
		// Step on rows from top to bottom
		uint8_t x = index * (FONT_8X5_WIDTH + 1) + i;
		for (j = 0; j < FONT_8X5_HEIGHT; j++)
		{
			// Draw pixel to "screen"
			uint8_t y = (line * (FONT_8X5_HEIGHT + 1)) + j;
			if (Font8x5[chr][i] & (1 << (7-j)))
			{
				SSD1306_drawPixel(x, y, WHITE);
			}
			else
			{
				SSD1306_drawPixel(x, y, BLACK);
			}
		}
	}
}



void Display_PrintFont12x8(uint8_t chr, uint8_t index, uint8_t line)
{
	// 12x8 pixel font
	uint8_t i;
	uint8_t j;

	// Step on rows from top to bottom
	for (i = 0; i < FONT_12X8_HEIGHT; i++)
	{
		// Step on column from left to right
		uint8_t y = (line * (FONT_12X8_HEIGHT + 1)) + i;
		for (j = 0; j < FONT_12X8_WIDTH; j++)
		{
			// Draw pixel to "screen"
			uint8_t x = index * (FONT_12X8_WIDTH + 1) + j;
			if (Font12x8[chr][i] & (1 << (7-j)))
			{
				SSD1306_drawPixel(x, y, WHITE);
			}
			else
			{
				SSD1306_drawPixel(x, y, BLACK);
			}
		}
	}

}



void Display_ChangeCarImage(void)
{
	uint8_t *img = NULL;

	if (CarActualState)
	{
		CarActualState = false;
		img = (uint8_t *)CarWheel1;
	}
	else
	{
		CarActualState = true;
		img = (uint8_t *)CarWheel2;
	}

	SSD1306_drawImage(16, 16, 24, 40, img);
	SSD1306_drawImage(16, 16, 96, 40, img);

	Display_Activate();
}



void Display_Test8x5Font(void)
{
	Display_Clear();

	Display_PrintString("Text example", 0, Font_8x5);
	Display_PrintString("Sari <3", 1, Font_8x5);
	Display_PrintString("0123456789", 2, Font_8x5);
	Display_PrintString("abcdefghijklm", 3, Font_8x5);
	Display_PrintString("nopqrstuvwxyz", 4, Font_8x5);
	Display_PrintString(",.;?-*_()[]{}&", 5, Font_8x5);
	Display_PrintString("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 6, Font_8x5);

	Display_Activate();
}



void Display_Test12x8Font(void)
{
	Display_Clear();

	Display_PrintString("0123456789", 0, Font_12x8);
	Display_PrintString("abcdefghijklm", 1, Font_12x8);
	Display_PrintString("nopqrtsuvwxyz", 2, Font_12x8);
	Display_PrintString(",.;?-*_()[]{}&", 3, Font_12x8);
	Display_PrintString("ABCDEFGHIJKLMNOPQ", 4, Font_12x8);

	Display_Activate();
}



void Display_Clear(void)
{
	SSD1306_clearDisplay();
}



void Display_Activate(void)
{
	SSD1306_display();
}



/**
 * \brief	Fill rectangle with color
 */
static void Display_FillRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
//#define BLACK 0
//#define WHITE 1
	uint8_t i;
	uint8_t j;

	//
	for (i=x; i<=x+width; i++)
	{
		for (j=y; j<=y+height; j++)
		{
			SSD1306_drawPixel(i, j, color);
		}
	}

}



void Display_LoadingInit(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	// Clear rectangle
	Display_FillRectangle(x, y, width, height, BLACK);

	// Init empty rectangle
	SSD1306_drawFastVLine(x, y, height, WHITE);
	SSD1306_drawFastVLine(x+width, y, height, WHITE);
	SSD1306_drawFastHLine(x, y, width, WHITE);
	SSD1306_drawFastHLine(x, y+height, width, WHITE);

	// 0 percent

}



void Display_LoadingPercent(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percent)
{
	// Fill rectangle
	uint8_t percent_real = (float)width*percent/100;
	Display_FillRectangle(x+2, y+2, percent_real, height-4, WHITE);
}



void Display_TestLoading(uint8_t percent)
{
	static uint8_t x = 20;
	static uint8_t y = 20;
	static uint8_t width = 80;
	static uint8_t height = 40;

	if (percent == 0)
	{
		Display_Clear();
		Display_LoadingInit(x,  y,  width, height);
	}
	Display_LoadingPercent(x, y, width, height, percent);

	Display_Activate();
}



#endif	// #ifdef CONFIG_MODULE_DISPLAY_ENABLE
