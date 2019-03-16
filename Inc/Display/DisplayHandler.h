/*
 *    DisplayHandler.h
 *    Created on:   2019-03-16
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */

#ifndef DISPLAYHANDLER_H_
#define DISPLAYHANDLER_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "Display_SSD1306.h"



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

void DisplayHandler_DrawPixel(uint8_t x, uint8_t y, Display_Color_t color);
void DisplayHandler_DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void DisplayHandler_DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void DisplayHandler_DrawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color);
void DisplayHandler_DrawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color);
void DisplayHandler_DrawImage(uint8_t setx, uint8_t sety, uint8_t sizex, uint8_t sizey, uint8_t *img);

void DisplayHandler_ClearDisplay(void);
void DisplayHandler_ShowDisplay(void);

void DisplayHandler_SendOnTerminal(void);



#endif /* DISPLAYHANDLER_H_ */
