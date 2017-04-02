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
#include "include.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/

void Display_PrintString(const char *str, uint8_t line, Font_Type font);
void Display_PrintFont8x5(uint8_t chr, uint8_t index, uint8_t line);
void Display_PrintFont12x8(uint8_t chr, uint8_t index, uint8_t line);

void Display_ChangeCarImage(void);
void Display_Test8x5Font(void);
void Display_Test12x8Font(void);

void Display_Clear(void);
void Display_Activate(void);



#endif /* MODULES_DISPLAY_H_ */
