/*
 *		Display.h
 *		Created on:		2017-04-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Display handling
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-04-02
 */

#ifndef MODULES_DISPLAY_H_
#define MODULES_DISPLAY_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK
#include "DateTime.h"
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
#include "Logic.h"
#endif


/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/

#define NO_FORMAT				(Display_NoFormat)



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef enum
{
	Font_Unknown,
#ifdef CONFIG_DISPLAY_FONT8X5_ENABLE
	Font_8x5,
#endif
#ifdef CONFIG_DISPLAY_FONT12X8_ENABLE
	Font_12x8,
#endif
#ifdef CONFIG_DISPLAY_FONT32X20_ENABLE
	Font_32x20,
#endif

	Font_Count
} FontType_t;


typedef struct
{
	uint32_t Format_Center:		1;
	uint32_t Format_Inverse:	1;
	uint32_t Format_Underline:	1;
} FontFormat_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern bool Display_CarAnimationDisable_flag;
const FontFormat_t Display_NoFormat;



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Display_PrintString(const char *str, uint8_t line, FontType_t font, FontFormat_t format);
void Display_PrintFont8x5(uint8_t chr, uint8_t index, uint8_t line, FontFormat_t format);
void Display_PrintFont12x8(uint8_t chr, uint8_t index, uint8_t line, FontFormat_t format);
void Display_PrintFont32x20(uint8_t chr, uint8_t index, uint8_t startposx, uint8_t startposy, FontFormat_t format);

void Display_Clear(void);
void Display_Activate(void);

void Display_TestLoading(uint8_t percent);
void Display_ChargeLoading(uint8_t percent);

void Display_LoadCarImage(void);
void Display_ChangeCarImage(void);

#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK
void Display_ShowLargeClock(Time_t *time);
void Display_ShowSmallClock(Time_t *time);
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
void Display_ShowClockHalf(Time_t *time, DisplayClock_ChangeState_t displayState);
#endif


void Display_Test8x5Font(void);
void Display_Test12x8Font(void);
void Display_Test32x20Font(void);
void Display_TestClock(void);



#endif /* MODULES_DISPLAY_H_ */
