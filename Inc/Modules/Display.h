/*
 *		Display.h
 *
 *		Created on:		2017. ápr. 2.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. ápr. 2.
 */

#ifndef MODULES_DISPLAY_H_
#define MODULES_DISPLAY_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_DISPLAY_SHOW_CLOCK
#include "DateTime.h"
#endif



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
	Font_Unknown,
#ifdef CONFIG_MODULE_DISPLAY_FONT8X5_ENABLE
	Font_8x5,
#endif
#ifdef CONFIG_MODULE_DISPLAY_FONT12X8_ENABLE
	Font_12x8,
#endif
#ifdef CONFIG_MODULE_DISPLAY_FONT32X20_ENABLE
	Font_32x20,
#endif

	Font_Count
} Font_Type;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern bool Display_CarAnimationDisable_flag;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Display_PrintString(const char *str, uint8_t line, Font_Type font);
void Display_PrintFont8x5(uint8_t chr, uint8_t index, uint8_t line);
void Display_PrintFont12x8(uint8_t chr, uint8_t index, uint8_t line);
void Display_PrintFont32x20(uint8_t chr, uint8_t index, uint8_t startposx, uint8_t startposy);

void Display_Clear(void);
void Display_Activate(void);

void Display_TestLoading(uint8_t percent);

void Display_LoadCarImage(void);
void Display_ChangeCarImage(void);

#ifdef CONFIG_MODULE_DISPLAY_SHOW_CLOCK
void Display_ShowClock(Time_t *time);
#endif

void Display_Test8x5Font(void);
void Display_Test12x8Font(void);
void Display_Test32x20Font(void);
void Display_TestClock(void);



#endif /* MODULES_DISPLAY_H_ */
