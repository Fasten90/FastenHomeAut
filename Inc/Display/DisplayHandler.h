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

void SSD1306_drawPixel(uint8_t x, uint8_t y, Display_Color_t color);
void SSD1306_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void SSD1306_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void SSD1306_drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color);
void SSD1306_drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color);

void SSD1306_clearDisplay(void);
void DisplayHandler_Display(void);

void Display_SendOnTerminal(void);



#endif /* DISPLAYHANDLER_H_ */
