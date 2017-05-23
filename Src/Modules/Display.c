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
#ifdef CONFIG_MODULE_DISPLAY_FONT32X20_ENABLE
#include "Font32x20.h"
#endif


#define DISPLAY_FONT32X20_CLOCK_START_POSITION_X		( ( 128 - 5*20)/2 )
#define DISPLAY_FONT32X20_CLOCK_START_POSITION_Y		( ( 64 - 1*32)/2 )


/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static uint8_t CarActualStateCnt = 0;
bool Display_CarAnimationDisable_flag = false;



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void Display_FillRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	Print ASCII text string to display
 */
void Display_PrintString(const char *str, uint8_t line, Font_Type font)
{

	uint8_t i;

	for (i = 0; str[i]; i++)
	{
		switch (font)
		{
#ifdef CONFIG_MODULE_DISPLAY_FONT8X5_ENABLE
			case Font_8x5:
				// Print ASCII character (0-127)
				Display_PrintFont8x5(str[i], i, line);
				break;
#endif
#ifdef CONFIG_MODULE_DISPLAY_FONT12X8_ENABLE
			case Font_12x8:
				// Print ASCII character (0-127)
				Display_PrintFont12x8(str[i], i, line);
				break;
#endif
#ifdef CONFIG_MODULE_DISPLAY_FONT32X20_ENABLE
			case Font_32x20:
				// Print numbers (for Clock)
				Display_PrintFont32x20(str[i], i,
					DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
					DISPLAY_FONT32X20_CLOCK_START_POSITION_Y);
				break;
#endif
			case Font_Unknown:
			case Font_Count:
			default:
				break;
		}
	}
}



#ifdef CONFIG_MODULE_DISPLAY_FONT8X5_ENABLE
/**
 * \brief	Print a 8x5 pixel (very small) character to display
 * \param	chr		- which character (ASCII - 0-127)
 * \param	index	- column
 * \param	line	- line / row
 */
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
#endif



#ifdef CONFIG_MODULE_DISPLAY_FONT12X8_ENABLE
/**
 * \brief	Print a 12x8 pixel character to display
 * \param	chr		- which character (ASCII - 0-127)
 * \param	index	- column
 * \param	line	- line / row
 */
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
#endif



#ifdef CONFIG_MODULE_DISPLAY_FONT32X20_ENABLE
/**
 * \brief	Print 32x20 font
 */
void Display_PrintFont32x20(uint8_t chr, uint8_t index, uint8_t startposx, uint8_t startposy)
{
	// 32x20 pixel font
	uint8_t i;
	uint8_t j;

	// Character selecting
	if (chr >= '0' && chr <= '9')
	{
		// 0-9 - Dependent with Font store (32x20)
		chr -= '0';
	}
	else if (chr == ':')
	{
		// ':'	- Dependent with Font store (32x20)
		chr = 10;
	}

	// Step on columns
	for (i = 0; i < FONT_32X20_WIDTH; i++)
	{
		// Step on rows from top to bottom
		uint8_t x = startposx + index * (FONT_32X20_WIDTH + 1) + i;
		for (j = 0; j < FONT_32X20_HEIGHT; j++)
		{
			// Draw pixel to "screen"
			uint8_t y = startposy + j;
			if (Font32x20[chr][i] & (1 << (31-j)))
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
#endif



/**
 * \brief	Clear display - make empty screen
 */
void Display_Clear(void)
{
	SSD1306_clearDisplay();
}



/**
 * \brief	Refresh Display
 */
void Display_Activate(void)
{
	SSD1306_display();
/*
#ifdef CONFIG_MODULE_DISPLAY_TEST_WITH_TERMINAL
	Display_SendOnTerminal();
#endif
*/
}



/**
 * \brief	Fill rectangle with color
 */
static void Display_FillRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
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


/*------------------------------------------------------------------------------
 *  							Loading screen
 *----------------------------------------------------------------------------*/


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
	static uint8_t x = 16;
	static uint8_t y = 56;
	static uint8_t width = 96;
	static uint8_t height = 8;

	if (percent == 0)
	{
		//Display_Clear();
		Display_LoadingInit(x,  y,  width, height);
	}
	Display_LoadingPercent(x, y, width, height, percent);

	Display_Activate();
}



/*------------------------------------------------------------------------------
 *  							Car images
 *----------------------------------------------------------------------------*/



/**
 * \brief	Load "Car" image to screen
 */
void Display_LoadCarImage(void)
{
	Display_Clear();

	SSD1306_drawImage(8, 8, 120, 48, (uint8_t *)Display_CarImage);

	Display_Activate();
}



/**
 * \brief	Change "Car's wheels"
 */
void Display_ChangeCarImage(void)
{
	uint8_t *img = NULL;

	if ((CarActualStateCnt % 3) == 0)
	{
		// 1.
		img = (uint8_t *)Image_CarWheel_1_16x16;
	}
	else if ((CarActualStateCnt % 3) == 1)
	{
		// 2.
		img = (uint8_t *)Image_CarWheel_2_16x16;
	}
	else
	{
		// 3.
		img = (uint8_t *)Image_CarWheel_3_16x16;
	}

	SSD1306_drawImage(24, 40, 16, 16, img);
	SSD1306_drawImage(96, 40, 16, 16, img);

	CarActualStateCnt++;

	Display_Activate();
}



#ifdef CONFIG_MODULE_DISPLAY_SHOW_CLOCK
/**
 * \brief	Display time (HH:MM)
 */
void Display_ShowClock(Time_t *time)
{
	// Set hour
	Display_PrintFont32x20(time->hour/10, 0,
			DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
			DISPLAY_FONT32X20_CLOCK_START_POSITION_Y);

	Display_PrintFont32x20(time->hour%10, 1,
			DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
			DISPLAY_FONT32X20_CLOCK_START_POSITION_Y);

	// ':'
	Display_PrintFont32x20(':', 2,
			DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
			DISPLAY_FONT32X20_CLOCK_START_POSITION_Y);

	// Set minute
	Display_PrintFont32x20(time->minute/10, 3,
			DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
			DISPLAY_FONT32X20_CLOCK_START_POSITION_Y);

	Display_PrintFont32x20(time->minute%10, 4,
			DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
			DISPLAY_FONT32X20_CLOCK_START_POSITION_Y);

	// Refresh display
	Display_Activate();
}
#endif



#ifdef CONFIG_MODULE_DISPLAY_TEST

#ifdef CONFIG_MODULE_DISPLAY_FONT8X5_ENABLE
/**
 * \brief	Test 8x5 fonts
 */
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
#endif



#ifdef CONFIG_MODULE_DISPLAY_FONT12X8_ENABLE
/**
 * \brief	Test 12x8 fonts
 */
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
#endif



#ifdef CONFIG_MODULE_DISPLAY_FONT32X20_ENABLE
/**
 * \brief	Test 32x20 fonts
 */
void Display_Test32x20Font(void)
{
	Display_Clear();

	Display_PrintString("12:34", 0, Font_32x20);

	Display_Activate();
}
#endif



#ifdef CONFIG_MODULE_DISPLAY_SHOW_CLOCK
/**
 * \brief	Test Clock show
 */
void Display_TestClock(void)
{
	uint16_t i;
	DateTime_t dateTime = { { 17, 5, 14 }, { 0, 0, 0 } };

	Display_Clear();


	for (i = 0; i < 24*60; i++)
	{
		Display_ShowClock(&dateTime.time);
		DelayMs(300);
		// Step 1 minute
		DateTime_Steps(&dateTime, 60);
	}

}
#endif

#endif	// #ifdef CONFIG_MODULE_DISPLAY_TEST



#endif	// #ifdef CONFIG_MODULE_DISPLAY_ENABLE
