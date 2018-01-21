/*
 *		DisplayImages.h
 *		Created on:		2017-04-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Display - images
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-04-02
 */

#ifndef DISPLAYIMAGES_H_
#define DISPLAYIMAGES_H_



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "compiler.h"



/*------------------------------------------------------------------------------
 *  Macros & definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

extern const uint8_t Display_Image_Car[120 * 48 / 8];

extern const uint8_t Display_Image_CarWheel_1_16x16[16 * 16 / 8];
extern const uint8_t Display_Image_CarWheel_2_16x16[16 * 16 / 8];
extern const uint8_t Display_Image_CarWheel_3_16x16[16 * 16 / 8];



/*------------------------------------------------------------------------------
 *  Global function declarations
 *----------------------------------------------------------------------------*/



#endif /* DISPLAYIMAGES_H_ */
